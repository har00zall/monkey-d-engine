
#include <random>
#include <limits>
#include <SDL3/SDL.h>
#include "Context.h"
#include "Core/OS/Time.h"
#include "Core/3D/Geometry.h"
#include "Core/3D/Material.h"
#include "Core/System/SystemManager.h"
#include "Systems/GraphicsSystem.h"
#include "Camera.h"
#include "Renderer.h"
#include "MeshRenderer.h"

using namespace MonkeyDEngine;

MeshRenderer::MeshRenderer(const char *meshPath)
{
    m_meshFilePath = meshPath;
}

void MeshRenderer::Start()
{
    Renderer::Start();

    // create the vertex buffer
    gpuVertexBufferInfo.size = 0;
    gpuVertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

    // create the index buffer
    gpuIndexBufferInfo.size = 0;
    gpuIndexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;

    // load geometry
    LoadMesh(m_meshFilePath.c_str(), m_mesh);
    gpuVertexBuffer = SDL_CreateGPUBuffer(g_Context.gpuDevice, &gpuVertexBufferInfo);
    gpuIndexBuffer = SDL_CreateGPUBuffer(g_Context.gpuDevice, &gpuIndexBufferInfo);

    // create uniform buffer
    gpuVertexUniformBufferInfo.size = sizeof(VertexUniformBufferObject);
    gpuVertexUniformBufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
    gpuVertexUniformBuffer = SDL_CreateGPUBuffer(g_Context.gpuDevice, &gpuVertexUniformBufferInfo);

    // create a transfer buffer to upload to the vertex buffer
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = (Uint32)(gpuVertexBufferInfo.size + gpuIndexBufferInfo.size);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer *gpuTransferBuffer = SDL_CreateGPUTransferBuffer(g_Context.gpuDevice, &transferInfo);

    // fill the transfer buffer
    Uint8 *mapped = (Uint8 *)SDL_MapGPUTransferBuffer(g_Context.gpuDevice, gpuTransferBuffer, false);

    SDL_memcpy(mapped, m_mesh.vertices.data(), gpuVertexBufferInfo.size);
    SDL_memcpy(mapped + gpuVertexBufferInfo.size, m_mesh.indices.data(), gpuIndexBufferInfo.size);
    SDL_UnmapGPUTransferBuffer(g_Context.gpuDevice, gpuTransferBuffer);

    // start a copy pass
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(g_Context.gpuDevice);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    // where is the data
    SDL_GPUTransferBufferLocation location{};
    location.offset = 0;
    location.transfer_buffer = gpuTransferBuffer;

    // where to upload the data
    SDL_GPUBufferRegion region{};
    region.buffer = gpuVertexBuffer;
    region.size = gpuVertexBufferInfo.size;
    region.offset = 0;

    // 1. upload the vertex data
    SDL_UploadToGPUBuffer(copyPass, &location, &region, false);

    // update the data for index buffer
    location.offset = gpuVertexBufferInfo.size;
    region.buffer = gpuIndexBuffer;
    region.size = gpuIndexBufferInfo.size;

    // 2. upload the index data
    SDL_UploadToGPUBuffer(copyPass, &location, &region, false);

    // end the copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    SDL_ReleaseGPUTransferBuffer(g_Context.gpuDevice, gpuTransferBuffer);

    // Setup material
    m_material->Setup();

    std::random_device rd;

    // Mersenne Twister engine seeded with random_device
    std::mt19937 gen(rd());

    // Define the range [min, max]
    int min = 1;
    int max = 100;

    // Uniform distribution in the given range
    std::uniform_int_distribution<> dist(min, max);

    // Generate and print a random number
    int randomNumber = dist(gen);
    m_randomRotationSpeed = (float)(max / randomNumber);
}

void MeshRenderer::Update()
{
    transform.Rotate({0.f, m_randomRotationSpeed * Time::deltaTime, 0.f});
}

void MeshRenderer::Render()
{
    // graphicsSystem = SystemManager::Instance().Get<GraphicsSystem>();

    // object matrices
    if (!graphicsSystem)
    {
        SDL_Log("Graphics System is Missing");
        return;
    }
    if (!g_Context.mainCamera)
    {
        SDL_Log("Camera on Graphics System is Missing");
        return;
    }
    // binding gpu pipeline
    m_material->Bind(graphicsSystem->gpuRenderPass.activeRenderPass);

    // binding vertex buffer
    SDL_GPUBufferBinding vertexBufferBindings[1];
    vertexBufferBindings[0].buffer = gpuVertexBuffer;
    vertexBufferBindings[0].offset = 0;
    SDL_BindGPUVertexBuffers(graphicsSystem->gpuRenderPass.activeRenderPass, 0, vertexBufferBindings, 1);

    // binding index buffer
    SDL_GPUBufferBinding indexBufferBindings;
    indexBufferBindings.buffer = gpuIndexBuffer;
    indexBufferBindings.offset = 0;
    SDL_BindGPUIndexBuffer(graphicsSystem->gpuRenderPass.activeRenderPass, &indexBufferBindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    FragmentUniformBufferData fragmentUniformBufferData{};
    fragmentUniformBufferData.viewPosition = g_Context.mainCamera->transform.GetPosition();
    SDL_PushGPUFragmentUniformData(graphicsSystem->gpuCommandBuffer, 0, &fragmentUniformBufferData, sizeof(FragmentUniformBufferData));

    VertexUniformBufferObject vertexUniformBufferObject{};
    vertexUniformBufferObject.viewProjection = g_Context.mainCamera->transform.GetViewProjectionMatrix();
    vertexUniformBufferObject.model = transform.GetModelMatrix();
    SDL_PushGPUVertexUniformData(graphicsSystem->gpuCommandBuffer, 0, &vertexUniformBufferObject, sizeof(VertexUniformBufferObject));

    SDL_DrawGPUIndexedPrimitives(graphicsSystem->gpuRenderPass.activeRenderPass, m_mesh.GetIndexCount(), 1, 0, 0, 0);
}

void MeshRenderer::OnDestroy()
{
    // release buffers
    Renderer::OnDestroy();

    m_material.reset();
}

void MeshRenderer::LoadMesh(const char *filePath, Mesh &outMesh)
{
    std::vector<Vertex> vertices;
    std::vector<Uint32> indices;
    if (!Geometry::LoadGLTF(filePath, vertices, indices))
    {
        SDL_Log("Failed to load model, aborting");
        return;
    }

    SDL_Log("Loaded %zu vertices, %zu indices", vertices.size(), indices.size());

    // create the vertex buffer
    gpuVertexBufferInfo.size += (Uint32)(vertices.size() * sizeof(Vertex));

    // create the index buffer
    gpuIndexBufferInfo.size += (Uint32)(indices.size() * sizeof(Uint32));

    outMesh.vertices = vertices;
    outMesh.indices = indices;
}
