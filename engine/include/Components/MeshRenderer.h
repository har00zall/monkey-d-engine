#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include "Components/Component.h"
#include "Core/3D/Geometry.h"

namespace MonkeyDEngine
{
    class GraphicsSystem;
    class MeshRenderer : public Component
    {
    protected:
        Mesh m_mesh;

        SDL_GPUBufferCreateInfo gpuVertexBufferInfo;
        SDL_GPUBuffer *gpuVertexBuffer;
        SDL_GPUBufferCreateInfo gpuIndexBufferInfo;
        SDL_GPUBuffer *gpuIndexBuffer;
        SDL_GPUBufferCreateInfo gpuVertexUniformBufferInfo;
        SDL_GPUBuffer *gpuVertexUniformBuffer;
        SDL_GPUTexture *m_texture;
        SDL_GPUSampler *m_textureSampler;

        std::shared_ptr<GraphicsSystem> graphicsSystem = nullptr;

        void LoadMesh(const char *filePath, Mesh &outMesh);

    public:
        MeshRenderer();
        MeshRenderer(Mesh mesh) { m_mesh = mesh; }

        ~MeshRenderer();

        virtual void Start() override;
        virtual void Update() override;
        virtual void Render() override;
        virtual void OnDestroy() override;
    };
};
