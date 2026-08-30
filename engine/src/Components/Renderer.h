#pragma once

#include <memory>
#include <unordered_map>
#include <SDL3/SDL.h>
#include "Components/Component.h"
#include "Core/3D/Geometry.h"

namespace MonkeyDEngine
{
    struct RenderContext;
    class GraphicsSystem;
    class Renderer : public Component
    {
    protected:
        GraphicsSystem *graphicsSystem = nullptr;

        void OnStart() override;
        void OnUpdate() override;

    public:
        Renderer() = default;
        virtual ~Renderer() = default;
        void OnDestroy() override;

        virtual void Render(RenderContext &renderContext);
    };
};
