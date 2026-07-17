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
        std::string m_meshFilePath;
        std::string m_textureFilePath;
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

        float m_randomRotationSpeed;

        void LoadMesh(const char *filePath, Mesh &outMesh);

    public:
        MeshRenderer();
        MeshRenderer(const char *meshPath);
        MeshRenderer(const char *meshPath, const char *texturePath);
        MeshRenderer(Mesh mesh) { m_mesh = mesh; }

        ~MeshRenderer();

        virtual void Start() override;
        virtual void Update() override;
        virtual void Render() override;
        virtual void OnDestroy() override;
    };
};
