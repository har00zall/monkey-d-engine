#include <SDL3/SDL.h>
#include "Core/System/SystemLocator.h"
#include "Core/3D/Geometry.h"
#include "Systems/GraphicsSystem.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"

using namespace MonkeyDEngine;

MeshRenderer::MeshRenderer()
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Start()
{
    auto graphicsSystem = SystemLocator::Instance().Get<GraphicsSystem>();
    graphicsSystem->gpuRenderPass.meshRenderers.push_back(this);

    // create the vertex buffer
    graphicsSystem->gpuVertexBufferInfo.size = 0;
    graphicsSystem->gpuVertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    SDL_Log("VertexBufferInfo has %d size", graphicsSystem->gpuVertexBufferInfo.size);

    // create the index buffer
    graphicsSystem->gpuIndexBufferInfo.size = 0;
    graphicsSystem->gpuIndexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    SDL_Log("indexBufferInfo has %d size", graphicsSystem->gpuIndexBufferInfo.size);

    // load geometry
    LoadMesh("assets/monkey.gltf", m_mesh);
    graphicsSystem->gpuVertexBuffer = SDL_CreateGPUBuffer(graphicsSystem->gpuDevice, &graphicsSystem->gpuVertexBufferInfo);
    graphicsSystem->gpuIndexBuffer = SDL_CreateGPUBuffer(graphicsSystem->gpuDevice, &graphicsSystem->gpuIndexBufferInfo);

    // create storage buffer
    graphicsSystem->gpuVertexUniformBufferInfo.size = sizeof(VertexUniformBufferObject);
    graphicsSystem->gpuVertexUniformBufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
    SDL_Log("gpuUniformBufferInfo has %d size", graphicsSystem->gpuVertexUniformBufferInfo.size);
    graphicsSystem->gpuVertexUniformBuffer = SDL_CreateGPUBuffer(graphicsSystem->gpuDevice, &graphicsSystem->gpuVertexUniformBufferInfo);

    // create a transfer buffer to upload to the vertex buffer
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = (Uint32)(graphicsSystem->gpuVertexBufferInfo.size + graphicsSystem->gpuIndexBufferInfo.size);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_Log("transferInfo has %d size", transferInfo.size);
    SDL_GPUTransferBuffer *gpuTransferBuffer = SDL_CreateGPUTransferBuffer(graphicsSystem->gpuDevice, &transferInfo);

    // fill the transfer buffer
    Uint8 *mapped = (Uint8 *)SDL_MapGPUTransferBuffer(graphicsSystem->gpuDevice, gpuTransferBuffer, false);

    SDL_memcpy(mapped, m_mesh.vertices.data(), graphicsSystem->gpuVertexBufferInfo.size);
    SDL_memcpy(mapped + graphicsSystem->gpuVertexBufferInfo.size, m_mesh.indices.data(), graphicsSystem->gpuIndexBufferInfo.size);
    SDL_UnmapGPUTransferBuffer(graphicsSystem->gpuDevice, gpuTransferBuffer);

    // start a copy pass
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(graphicsSystem->gpuDevice);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    // where is the data
    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = gpuTransferBuffer;
    location.offset = 0;

    // where to upload the data
    SDL_GPUBufferRegion region{};
    region.buffer = graphicsSystem->gpuVertexBuffer;
    region.size = graphicsSystem->gpuVertexBufferInfo.size;
    region.offset = 0;

    // upload the vertex data
    SDL_Log("[Started] Uploading vertex data to GPU");
    SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
    SDL_Log("[End] Uploaded vertex data to GPU");

    // update the data for index buffer
    location.offset = graphicsSystem->gpuVertexBufferInfo.size;
    region.buffer = graphicsSystem->gpuIndexBuffer;
    region.size = graphicsSystem->gpuIndexBufferInfo.size;

    // upload the index data
    SDL_Log("[Started] Uploading index data to GPU");
    SDL_UploadToGPUBuffer(copyPass, &location, &region, false);

    // end the copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_Log("[End] Uploaded index data to GPU");
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    SDL_ReleaseGPUTransferBuffer(graphicsSystem->gpuDevice, gpuTransferBuffer);
}

void MeshRenderer::Update()
{
}

void MeshRenderer::Render()
{
    auto graphicsSystem = SystemLocator::Instance().Get<GraphicsSystem>();

    // object matrices
    SDL_Log("Creating vertex buffer object");
    if (!graphicsSystem)
    {
        SDL_Log("Graphics System is Missing");
        return;
    }
    if (!graphicsSystem->mainCamera)
    {
        SDL_Log("Camera on Graphics System is Missing");
        return;
    }
    FragmentUniformBufferData fragmentUniformBufferData{};
    fragmentUniformBufferData.viewPosition = graphicsSystem->mainCamera->GetTransform().position;
    SDL_PushGPUFragmentUniformData(graphicsSystem->gpuCommandBuffer, 0, &fragmentUniformBufferData, sizeof(FragmentUniformBufferData));

    VertexUniformBufferObject vertexUniformBufferObject{};
    SDL_Log("assign viewProjection to vertex buffer object");
    vertexUniformBufferObject.viewProjection = graphicsSystem->mainCamera->GetTransform().GetViewProjectionMatrix();
    SDL_Log("assign model to vertex buffer object");
    vertexUniformBufferObject.model = m_transform.GetModelMatrix();
    SDL_Log("push vertex buffer object");
    SDL_PushGPUVertexUniformData(graphicsSystem->gpuCommandBuffer, 0, &vertexUniformBufferObject, sizeof(VertexUniformBufferObject));

    SDL_DrawGPUIndexedPrimitives(graphicsSystem->gpuRenderPass.activeRenderPass, m_mesh.GetIndexCount(), 1, 0, 0, 0);
}

void MeshRenderer::LoadMesh(const char *filePath, Mesh &outMesh)
{
    auto graphicsSystem = SystemLocator::Instance().Get<GraphicsSystem>();
    std::vector<Vertex> vertices;
    std::vector<Uint32> indices;
    if (!Geometry::LoadGLTF(filePath, vertices, indices))
    {
        SDL_Log("Failed to load model, aborting");
        return;
    }

    SDL_Log("Loaded %zu vertices, %zu indices", vertices.size(), indices.size());

    // create the vertex buffer
    graphicsSystem->gpuVertexBufferInfo.size += (Uint32)(vertices.size() * sizeof(Vertex));
    SDL_Log("Updated VertexBufferInfo has %d size", graphicsSystem->gpuVertexBufferInfo.size);

    // create the index buffer
    graphicsSystem->gpuIndexBufferInfo.size += (Uint32)(indices.size() * sizeof(Uint32));
    SDL_Log("Updated indexBufferInfo has %d size", graphicsSystem->gpuIndexBufferInfo.size);

    outMesh.vertices = vertices;
    outMesh.indices = indices;
}
