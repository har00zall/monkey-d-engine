#pragma once

#include <memory>
#include <string>
#include <SDL3/SDL.h>
#include "Components/Renderer.h"
#include "Core/3D/Geometry.h"

namespace MonkeyDEngine
{
    class GraphicsSystem;
    class MeshRenderer : public Renderer
    {
    protected:
        std::string m_meshFilePath;
        std::string m_textureFilePath;
        Mesh m_mesh;
        SDL_GPUTexture *m_texture;
        SDL_GPUSampler *m_textureSampler;

        float m_randomRotationSpeed;

        void LoadMesh(const char *filePath, Mesh &outMesh);

    public:
        MeshRenderer() = default;
        MeshRenderer(const char *meshPath);
        MeshRenderer(const char *meshPath, const char *texturePath);
        MeshRenderer(Mesh mesh) { m_mesh = mesh; }

        virtual ~MeshRenderer() = default;

        void Start() override;
        void Update() override;
        void Render() override;
        void OnDestroy() override;
    };
};
