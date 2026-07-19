#pragma once

#include <memory>
#include <string>
#include <SDL3/SDL.h>
#include "Components/Renderer.h"
#include "Core/3D/Geometry.h"
#include "Core/3D/Material.h"

namespace MonkeyDEngine
{
    class GraphicsSystem;
    class MeshRenderer : public Renderer
    {
    protected:
        std::string m_meshFilePath;
        Mesh m_mesh;
        std::shared_ptr<Material> m_material;

        float m_randomRotationSpeed;

        void LoadMesh(const char *filePath, Mesh &outMesh);

    public:
        MeshRenderer() = default;
        MeshRenderer(const char *meshPath);
        MeshRenderer(Mesh mesh) { m_mesh = mesh; }

        virtual ~MeshRenderer() = default;

        inline void SetMesh(const char *meshPath) { m_meshFilePath = meshPath; }
        inline void SetMaterial(std::shared_ptr<Material> material) { m_material = material; }

        void Start() override;
        void Update() override;
        void Render() override;
        void OnDestroy() override;
    };
};
