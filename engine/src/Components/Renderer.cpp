
#include <memory>
#include <SDL3/SDL.h>
#include "Core/System/SystemManager.h"
#include "Systems/GraphicsSystem.h"
#include "Context.h"
#include "Renderer.h"

using namespace MonkeyDEngine;

void Renderer::Start()
{
    graphicsSystem = SystemManager::Instance().GetSystem<GraphicsSystem>();
    graphicsSystem->gpuRenderPass.AddRenderer(this);
}

void Renderer::Update()
{
}

void Renderer::Render(RenderContext &renderContext)
{
}

void Renderer::OnDestroy()
{
    graphicsSystem->gpuRenderPass.RemoveRenderer(this);
}
