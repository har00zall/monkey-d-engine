#pragma once

#include <vector>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Core/System/SystemBase.h"

namespace MonkeyDEngine
{
    class Renderer;
    struct VertexUniformBufferObject
    {
        glm::mat4 viewProjection;
        glm::mat4 model;
    };

    struct FragmentUniformBufferData
    {
        glm::vec3 viewPosition;
    };

    struct RenderPassData
    {
        SDL_GPURenderPass *activeRenderPass;
        std::vector<Renderer *> renderers;
    };

    class GraphicsSystem : public SystemBase
    {
    public:
        ~GraphicsSystem() = default;

        SDL_GPUDevice *gpuDevice;
        SDL_GPUTexture *depthTexture; // z-buffer

        SDL_GPUGraphicsPipeline *gpuGraphicsPipeline;
        SDL_GPUCommandBuffer *gpuCommandBuffer;
        RenderPassData gpuRenderPass{};

        int Render3D();
        void CreateDepthTexture();

    protected:
        void OnStartSystem() override;
        void OnStopSystem() override;

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
