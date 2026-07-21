#pragma once

#define MAX_INSTANCE 64

#include <vector>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Core/System/SystemBase.h"

namespace MonkeyDEngine
{
    class Shader;
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

    struct RendererData
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

    struct ShaderData
    {
        std::vector<Shader *> shaders;
        void AddShader(Shader *newShader)
        {
            shaders.push_back(newShader);
        }
        void RemoveShader(Shader *shader)
        {
            shaders.erase(
                std::remove(shaders.begin(), shaders.end(), shader),
                shaders.end());
        }
    };

    class GraphicsSystem : public SystemBase
    {
    public:
        ~GraphicsSystem() = default;

        SDL_GPUTexture *depthTexture; // z-buffer

        SDL_GPUCommandBuffer *gpuCommandBuffer;
        RendererData gpuRendererData{};
        ShaderData gpuShaderData{};

        int Render3D();
        void CreateDepthTexture();

    protected:
        void OnStartSystem() override;
        void OnStopSystem() override;
    };
}
