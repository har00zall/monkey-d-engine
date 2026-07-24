
#include <random>
#include <limits>
#include <unordered_map>
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

    MeshBufferData *meshBufferData = nullptr;
    auto it = m_meshBufferDataMap.find(m_meshFilePath.c_str());
    if (it == m_meshBufferDataMap.end()) // creating new mesh instance
    {
        meshBufferData = new MeshBufferData();

        SDL_Log("Creating new Mesh Instance");
        // load geometry
        LoadMesh(m_meshFilePath.c_str(), meshBufferData->mesh);

        // create the vertex buffer
        meshBufferData->gpuVertexBufferInfo.size = 0;
        meshBufferData->gpuVertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

        // create the index buffer
        meshBufferData->gpuIndexBufferInfo.size = 0;
        meshBufferData->gpuIndexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;

        m_meshBufferDataMap[m_meshFilePath.c_str()] = meshBufferData;
    }
    else
    {
        SDL_Log("Found mesh instance");
        meshBufferData = it->second;

        if (meshBufferData->gpuVertexBuffer)
            SDL_ReleaseGPUBuffer(g_Context.gpuDevice, meshBufferData->gpuVertexBuffer);
        if (meshBufferData->gpuIndexBuffer)
            SDL_ReleaseGPUBuffer(g_Context.gpuDevice, meshBufferData->gpuIndexBuffer);
        if (meshBufferData->gpuVertexUniformBuffer)
            SDL_ReleaseGPUBuffer(g_Context.gpuDevice, meshBufferData->gpuVertexUniformBuffer);
    }

    meshBufferData->renderers.push_back(this);

    SDL_Log("Found mesh instance, Updating Buffer");
    // create the vertex buffer
    meshBufferData->gpuVertexBufferInfo.size = (Uint32)(meshBufferData->mesh.vertices.size() * sizeof(Vertex));
    meshBufferData->gpuVertexBuffer = SDL_CreateGPUBuffer(g_Context.gpuDevice, &meshBufferData->gpuVertexBufferInfo);

    // create the index buffer
    meshBufferData->gpuIndexBufferInfo.size = (Uint32)(meshBufferData->mesh.indices.size() * sizeof(Uint32));
    meshBufferData->gpuIndexBuffer = SDL_CreateGPUBuffer(g_Context.gpuDevice, &meshBufferData->gpuIndexBufferInfo);

    // create uniform buffer
    meshBufferData->gpuVertexUniformBufferInfo.size = sizeof(VertexUniformBufferObject);
    meshBufferData->gpuVertexUniformBufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
    meshBufferData->gpuVertexUniformBuffer = SDL_CreateGPUBuffer(g_Context.gpuDevice, &meshBufferData->gpuVertexUniformBufferInfo);

    SDL_Log("Creating Transfer Buffers");

    // create a transfer buffer to upload to the vertex buffer
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = (Uint32)(meshBufferData->gpuVertexBufferInfo.size + meshBufferData->gpuIndexBufferInfo.size);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_Log("Transfer Buffer Size: %d", transferInfo.size);
    SDL_GPUTransferBuffer *gpuTransferBuffer = SDL_CreateGPUTransferBuffer(g_Context.gpuDevice, &transferInfo);

    SDL_Log("Mapping Transfer Data");
    // fill the transfer buffer
    Uint8 *mapped = (Uint8 *)SDL_MapGPUTransferBuffer(g_Context.gpuDevice, gpuTransferBuffer, false);

    SDL_memcpy(mapped, meshBufferData->mesh.vertices.data(), meshBufferData->gpuVertexBufferInfo.size);
    SDL_memcpy(mapped + meshBufferData->gpuVertexBufferInfo.size, meshBufferData->mesh.indices.data(), meshBufferData->gpuIndexBufferInfo.size);
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
    region.buffer = meshBufferData->gpuVertexBuffer;
    region.size = meshBufferData->gpuVertexBufferInfo.size;
    region.offset = 0;

    // 1. upload the vertex data
    SDL_UploadToGPUBuffer(copyPass, &location, &region, false);

    // update the data for index buffer
    location.offset = meshBufferData->gpuVertexBufferInfo.size;
    region.buffer = meshBufferData->gpuIndexBuffer;
    region.size = meshBufferData->gpuIndexBufferInfo.size;

    // 2. upload the index data
    SDL_UploadToGPUBuffer(copyPass, &location, &region, false);

    // end the copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    SDL_ReleaseGPUTransferBuffer(g_Context.gpuDevice, gpuTransferBuffer);

    SDL_Log("Transfer Completed");

    // Setup material
    SDL_Log("Setting up material");
    m_material->Setup();
    SDL_Log("Material Set");

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

void MeshRenderer::Render(RenderContext &renderContext)
{
    // SDL_Log("Start Rendering Mesh");
    MeshBufferData *meshBufferData;
    auto it = m_meshBufferDataMap.find(m_meshFilePath.c_str());
    if (it == m_meshBufferDataMap.end()) // no instance yet
    {
        SDL_Log("Cant find mesh instance");
        return;
    }

    meshBufferData = it->second;

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
    // SDL_Log("Binding material");
    m_material->Bind(renderContext.renderPass);

    // binding vertex buffer
    // SDL_Log("Binding Vertex Buffer");
    SDL_GPUBufferBinding vertexBufferBindings[1];
    vertexBufferBindings[0].buffer = meshBufferData->gpuVertexBuffer;
    vertexBufferBindings[0].offset = 0;
    SDL_BindGPUVertexBuffers(renderContext.renderPass, 0, vertexBufferBindings, 1);

    // binding index buffer
    // SDL_Log("Binding index Buffer");
    SDL_GPUBufferBinding indexBufferBindings;
    indexBufferBindings.buffer = meshBufferData->gpuIndexBuffer;
    indexBufferBindings.offset = 0;
    SDL_BindGPUIndexBuffer(renderContext.renderPass, &indexBufferBindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    // SDL_Log("push Fragment Uniform Data");
    FragmentUniformBufferData fragmentUniformBufferData{};
    fragmentUniformBufferData.viewPosition = g_Context.mainCamera->transform.GetPosition();
    SDL_PushGPUFragmentUniformData(graphicsSystem->gpuCommandBuffer, 0, &fragmentUniformBufferData, sizeof(FragmentUniformBufferData));
}

void MeshRenderer::OnDestroy()
{
    // release buffers
    Renderer::OnDestroy();

    auto it = m_meshBufferDataMap.find(m_meshFilePath);
    if (it != m_meshBufferDataMap.end())
    {
        SDL_Log("Release GPU Buffers For Mesh: %s", m_meshFilePath.c_str());
        auto meshBufferData = it->second;

        if (meshBufferData->gpuVertexBuffer)
            SDL_ReleaseGPUBuffer(g_Context.gpuDevice, meshBufferData->gpuVertexBuffer);
        if (meshBufferData->gpuIndexBuffer)
            SDL_ReleaseGPUBuffer(g_Context.gpuDevice, meshBufferData->gpuIndexBuffer);
        if (meshBufferData->gpuVertexUniformBuffer)
            SDL_ReleaseGPUBuffer(g_Context.gpuDevice, meshBufferData->gpuVertexUniformBuffer);

        m_meshBufferDataMap.erase(m_meshFilePath);
    }

    // m_material->Dispose();
    m_material = nullptr;
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

    outMesh.vertices = vertices;
    outMesh.indices = indices;
}
