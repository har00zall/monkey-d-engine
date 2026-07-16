#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Components/Camera.h"
#include "Core/System/SystemBase.h"

namespace MonkeyDEngine
{
    struct VertexUniformBufferObject
    {
        glm::mat4 viewProjection;
        glm::mat4 model;
    };

    struct FragmentUniformBufferData
    {
        glm::vec3 viewPosition;
    };

    class GraphicsSystem : public SystemBase
    {
    public:
        SDL_GPUDevice *gpuDevice;

        SDL_GPUBufferCreateInfo gpuVertexBufferInfo;
        SDL_GPUBuffer *gpuVertexBuffer;
        SDL_GPUBufferCreateInfo gpuIndexBufferInfo;
        SDL_GPUBuffer *gpuIndexBuffer;
        SDL_GPUBufferCreateInfo gpuVertexUniformBufferInfo;
        SDL_GPUBuffer *gpuVertexUniformBuffer;

        SDL_GPUTexture *depthTexture; // z-buffer

        SDL_GPUGraphicsPipeline *gpuGraphicsPipeline;

        Camera *mainCamera = new Camera();

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
