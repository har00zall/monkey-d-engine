#pragma once

#include <memory>
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include "Components/Renderer.h"
#include "Core/3D/Geometry.h"
#include "Core/3D/Material.h"

namespace MonkeyDEngine
{
    struct MeshBufferData
    {
        Mesh mesh;
        SDL_GPUBufferCreateInfo gpuVertexBufferInfo{};
        SDL_GPUBuffer *gpuVertexBuffer;
        SDL_GPUBufferCreateInfo gpuIndexBufferInfo{};
        SDL_GPUBuffer *gpuIndexBuffer;
        SDL_GPUBufferCreateInfo gpuVertexUniformBufferInfo{};
        SDL_GPUBuffer *gpuVertexUniformBuffer;

        std::vector<Renderer *> renderers;
    };

    class MeshRenderer : public Renderer
    {
    protected:
        std::string m_meshFilePath;
        std::shared_ptr<Material> m_material;

        float m_randomRotationSpeed;

        void LoadMesh(const char *filePath, Mesh &outMesh);

    public:
        inline static std::unordered_map<std::string, MeshBufferData *> m_meshBufferDataMap;

        MeshRenderer() = default;
        MeshRenderer(const char *meshPath);

        virtual ~MeshRenderer() = default;

        inline void SetMesh(const char *meshPath) { m_meshFilePath = meshPath; }
        inline void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }

        void Start() override;
        void Update() override;
        void Render(RenderContext &renderContext) override;
        void OnDestroy() override;
    };
};
