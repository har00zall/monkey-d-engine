#include <SDL3/SDL.h>
#include "Context.h"
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
    graphicsSystem = SystemLocator::Instance().Get<GraphicsSystem>();
    graphicsSystem->gpuRenderPass.meshRenderers.push_back(this);

    // create the vertex buffer
    gpuVertexBufferInfo.size = 0;
    gpuVertexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    SDL_Log("VertexBufferInfo has %d size", gpuVertexBufferInfo.size);

    // create the index buffer
    gpuIndexBufferInfo.size = 0;
    gpuIndexBufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    SDL_Log("indexBufferInfo has %d size", gpuIndexBufferInfo.size);

    // load geometry
    LoadMesh("assets/monkey.gltf", m_mesh);
    gpuVertexBuffer = SDL_CreateGPUBuffer(graphicsSystem->gpuDevice, &gpuVertexBufferInfo);
    gpuIndexBuffer = SDL_CreateGPUBuffer(graphicsSystem->gpuDevice, &gpuIndexBufferInfo);

    // Load texture
    SDL_Surface *surface = SDL_LoadPNG("assets/monkey.png");
    SDL_Surface *rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);

    // create texture
    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.width = rgbaSurface->w;
    textureInfo.height = rgbaSurface->h;
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    m_texture = SDL_CreateGPUTexture(graphicsSystem->gpuDevice, &textureInfo);
    Uint32 textureSize = rgbaSurface->pitch * rgbaSurface->h;

    // create sampler
    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    m_textureSampler = SDL_CreateGPUSampler(graphicsSystem->gpuDevice, &samplerInfo);

    // create uniform buffer
    gpuVertexUniformBufferInfo.size = sizeof(VertexUniformBufferObject);
    gpuVertexUniformBufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
    gpuVertexUniformBuffer = SDL_CreateGPUBuffer(graphicsSystem->gpuDevice, &gpuVertexUniformBufferInfo);

    // create a transfer buffer to upload to the vertex buffer
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = (Uint32)(gpuVertexBufferInfo.size + gpuIndexBufferInfo.size + textureSize);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_Log("transferInfo has %d size", transferInfo.size);
    SDL_GPUTransferBuffer *gpuTransferBuffer = SDL_CreateGPUTransferBuffer(graphicsSystem->gpuDevice, &transferInfo);

    // fill the transfer buffer
    Uint8 *mapped = (Uint8 *)SDL_MapGPUTransferBuffer(graphicsSystem->gpuDevice, gpuTransferBuffer, false);

    SDL_memcpy(mapped, rgbaSurface->pixels, textureSize);
    SDL_memcpy(mapped + textureSize, m_mesh.vertices.data(), gpuVertexBufferInfo.size);
    SDL_memcpy(mapped + textureSize + gpuVertexBufferInfo.size, m_mesh.indices.data(), gpuIndexBufferInfo.size);
    SDL_UnmapGPUTransferBuffer(graphicsSystem->gpuDevice, gpuTransferBuffer);

    // start a copy pass
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(graphicsSystem->gpuDevice);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    // data for texture
    SDL_GPUTextureTransferInfo textureTransferInfo{};
    textureTransferInfo.offset = 0;
    textureTransferInfo.transfer_buffer = gpuTransferBuffer;

    SDL_GPUTextureRegion textureTransferRegion{};
    textureTransferRegion.texture = m_texture;
    textureTransferRegion.w = rgbaSurface->w;
    textureTransferRegion.h = rgbaSurface->h;
    textureTransferRegion.d = 1;

    // 3. upload texture
    SDL_UploadToGPUTexture(copyPass, &textureTransferInfo, &textureTransferRegion, false);

    // where is the data
    SDL_GPUTransferBufferLocation location{};
    location.offset = textureSize;
    location.transfer_buffer = gpuTransferBuffer;

    // where to upload the data
    SDL_GPUBufferRegion region{};
    region.buffer = gpuVertexBuffer;
    region.size = gpuVertexBufferInfo.size;
    region.offset = 0;

    // 1. upload the vertex data
    SDL_Log("[Started] Uploading vertex data to GPU");
    SDL_UploadToGPUBuffer(copyPass, &location, &region, false);
    SDL_Log("[End] Uploaded vertex data to GPU");

    // update the data for index buffer
    location.offset = textureSize + gpuVertexBufferInfo.size;
    region.buffer = gpuIndexBuffer;
    region.size = gpuIndexBufferInfo.size;

    // 2. upload the index data
    SDL_Log("[Started] Uploading index data to GPU");
    SDL_UploadToGPUBuffer(copyPass, &location, &region, false);

    // end the copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_Log("[End] Uploaded index data to GPU");
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    SDL_ReleaseGPUTransferBuffer(graphicsSystem->gpuDevice, gpuTransferBuffer);

    // Clean up RAM resources
    SDL_DestroySurface(rgbaSurface);
}

void MeshRenderer::Update()
{
}

void MeshRenderer::Render()
{
    // graphicsSystem = SystemLocator::Instance().Get<GraphicsSystem>();

    // object matrices
    SDL_Log("Creating vertex buffer object");
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

    // binding vertex buffer
    SDL_GPUBufferBinding vertexBufferBindings[1];
    vertexBufferBindings[0].buffer = gpuVertexBuffer;
    vertexBufferBindings[0].offset = 0;
    SDL_Log("[Started] Binding Vertex Buffer");
    SDL_BindGPUVertexBuffers(graphicsSystem->gpuRenderPass.activeRenderPass, 0, vertexBufferBindings, 1);

    // binding index buffer
    SDL_GPUBufferBinding indexBufferBindings;
    indexBufferBindings.buffer = gpuIndexBuffer;
    indexBufferBindings.offset = 0;
    SDL_Log("[Started] Binding Index Buffer");
    SDL_BindGPUIndexBuffer(graphicsSystem->gpuRenderPass.activeRenderPass, &indexBufferBindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    SDL_GPUTextureSamplerBinding textureBindingInfo{};
    textureBindingInfo.sampler = m_textureSampler;
    textureBindingInfo.texture = m_texture;
    SDL_BindGPUFragmentSamplers(graphicsSystem->gpuRenderPass.activeRenderPass, 0, &textureBindingInfo, 1);

    FragmentUniformBufferData fragmentUniformBufferData{};
    fragmentUniformBufferData.viewPosition = g_Context.mainCamera->GetTransform().position;
    SDL_PushGPUFragmentUniformData(graphicsSystem->gpuCommandBuffer, 0, &fragmentUniformBufferData, sizeof(FragmentUniformBufferData));

    VertexUniformBufferObject vertexUniformBufferObject{};
    SDL_Log("assign viewProjection to vertex buffer object");
    vertexUniformBufferObject.viewProjection = g_Context.mainCamera->GetTransform().GetViewProjectionMatrix();
    SDL_Log("assign model to vertex buffer object");
    vertexUniformBufferObject.model = m_transform.GetModelMatrix();
    SDL_Log("push vertex buffer object");
    SDL_PushGPUVertexUniformData(graphicsSystem->gpuCommandBuffer, 0, &vertexUniformBufferObject, sizeof(VertexUniformBufferObject));

    SDL_DrawGPUIndexedPrimitives(graphicsSystem->gpuRenderPass.activeRenderPass, m_mesh.GetIndexCount(), 1, 0, 0, 0);
}

void MeshRenderer::OnDestroy()
{
    // release buffers
    SDL_ReleaseGPUBuffer(graphicsSystem->gpuDevice, gpuVertexBuffer);
    SDL_ReleaseGPUBuffer(graphicsSystem->gpuDevice, gpuIndexBuffer);
    SDL_ReleaseGPUBuffer(graphicsSystem->gpuDevice, gpuVertexUniformBuffer);
    SDL_ReleaseGPUSampler(graphicsSystem->gpuDevice, m_textureSampler);
    SDL_ReleaseGPUTexture(graphicsSystem->gpuDevice, m_texture);
}

void MeshRenderer::LoadMesh(const char *filePath, Mesh &outMesh)
{
    // graphicsSystem = SystemLocator::Instance().Get<GraphicsSystem>();
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
    SDL_Log("Updated VertexBufferInfo has %d size", gpuVertexBufferInfo.size);

    // create the index buffer
    gpuIndexBufferInfo.size += (Uint32)(indices.size() * sizeof(Uint32));
    SDL_Log("Updated indexBufferInfo has %d size", gpuIndexBufferInfo.size);

    outMesh.vertices = vertices;
    outMesh.indices = indices;
}
