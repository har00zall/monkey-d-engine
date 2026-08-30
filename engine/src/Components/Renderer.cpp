
#include <memory>
#include <SDL3/SDL.h>
#include "Core/System/SystemManager.h"
#include "Systems/GraphicsSystem.h"
#include "Context.h"
#include "Renderer.h"

using namespace MonkeyDEngine;

void Renderer::OnStart()
{
    graphicsSystem = SystemManager::Instance().GetSystem<GraphicsSystem>();
    graphicsSystem->gpuRendererData.AddRenderer(this);
}

void Renderer::OnUpdate()
{
}

void Renderer::Render(RenderContext &renderContext)
{
}

void Renderer::OnDestroy()
{
    graphicsSystem->gpuRendererData.RemoveRenderer(this);
}
