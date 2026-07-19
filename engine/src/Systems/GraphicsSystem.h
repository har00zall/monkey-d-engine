#pragma once

#define MAX_INSTANCE 64

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
        glm::mat4 model[MAX_INSTANCE];
    };

    struct FragmentUniformBufferData
    {
        glm::vec3 viewPosition;
    };

    struct RenderContext
    {
        SDL_GPURenderPass *renderPass;
        VertexUniformBufferObject vertexUniformBufferObject{};

        int currentModelIndex;
    };

    struct RenderPassData
    {
        std::vector<Renderer *> renderers;

        void AddRenderer(Renderer *newRenderer)
        {
            renderers.push_back(newRenderer);
        }
        void RemoveRenderer(Renderer *renderer)
        {
            renderers.erase(
                std::remove(renderers.begin(), renderers.end(), renderer),
                renderers.end());
        }
    };

    class GraphicsSystem : public SystemBase
    {
    public:
        ~GraphicsSystem() = default;

        SDL_GPUTexture *depthTexture; // z-buffer

        SDL_GPUCommandBuffer *gpuCommandBuffer;
        RenderPassData gpuRenderPass{};

        int Render3D();
        void CreateDepthTexture();

    protected:
        void OnStartSystem() override;
        void OnStopSystem() override;
    };
}
