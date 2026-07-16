#pragma once

#include <SDL3/SDL.h>
#include "Core/System/SystemBase.h"

namespace MonkeyDEngine
{
    class GraphicsSystem : public SystemBase
    {
    public:
        SDL_GPUDevice *gpuDevice;

        SDL_GPUBufferCreateInfo gpuVertexBufferInfo;
        SDL_GPUBuffer *gpuVertexBuffer;
        SDL_GPUBufferCreateInfo gpuIndexBufferInfo;
        SDL_GPUBuffer *gpuIndexBuffer;
        SDL_GPUBufferCreateInfo gpuStorageBufferInfo;
        SDL_GPUBuffer *gpuStorageBuffer;
        SDL_GPUTransferBufferCreateInfo gpuStorageTransferBufferInfo;
        SDL_GPUTransferBuffer *gpuStorageTransferBuffer;

        SDL_GPUTexture *depthTexture; // z-buffer

        SDL_GPUGraphicsPipeline *gpuGraphicsPipeline;

    protected:
        void OnStartSystem() override;
        void OnStopSystem() override;

        SDL_GPUGraphicsPipeline *CreateGraphicsPipeline();
        int Render3D();

        SDL_GPUShader *CreateShader(
            const char *shaderFilePath,
            SDL_GPUShaderFormat shaderFormat,
            SDL_GPUShaderStage shaderStage,
            Uint32 num_sampler = 0,
            Uint32 num_storage_buffers = 0,
            Uint32 num_storage_textures = 0,
            Uint32 num_uniform_buffers = 0);
    };
}
