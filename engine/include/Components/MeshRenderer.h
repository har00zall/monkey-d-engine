#pragma once

#include "Components/Component.h"
#include "Core/3D/Geometry.h"

namespace MonkeyDEngine
{
    class GraphicsSystem;
    class MeshRenderer : public Component
    {
    protected:
        Transform m_transform{};
        Mesh m_mesh;

        // std::shared_ptr<GraphicsSystem> graphicsSystem = nullptr;

        void LoadMesh(const char *filePath, Mesh &outMesh);

    public:
        MeshRenderer();
        MeshRenderer(Mesh mesh) { m_mesh = mesh; }

        ~MeshRenderer();

        Transform GetTransform() const { return m_transform; }

        virtual void Start() override;
        virtual void Update() override;
        virtual void Render() override;
    };
};
