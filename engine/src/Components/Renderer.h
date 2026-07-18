#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include "Components/Component.h"
#include "Core/3D/Geometry.h"

namespace MonkeyDEngine
{
    class GraphicsSystem;
    class Renderer : public Component
    {
    protected:
        SDL_GPUBufferCreateInfo gpuVertexBufferInfo;
        SDL_GPUBuffer *gpuVertexBuffer;
        SDL_GPUBufferCreateInfo gpuIndexBufferInfo;
        SDL_GPUBuffer *gpuIndexBuffer;
        SDL_GPUBufferCreateInfo gpuVertexUniformBufferInfo;
        SDL_GPUBuffer *gpuVertexUniformBuffer;

        GraphicsSystem *graphicsSystem = nullptr;

    public:
        Renderer() = default;
        virtual ~Renderer() = default;

        void Start() override;
        void Update() override;
        void OnDestroy() override;

        virtual void Render();
    };
};
