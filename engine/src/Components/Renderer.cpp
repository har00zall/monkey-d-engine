
#include <memory>
#include <SDL3/SDL.h>
#include "Core/System/SystemManager.h"
#include "Systems/GraphicsSystem.h"
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

void Renderer::Render()
{
}

void Renderer::OnDestroy()
{
    graphicsSystem->gpuRenderPass.RemoveRenderer(this);

    // release buffers
    SDL_ReleaseGPUBuffer(graphicsSystem->gpuDevice, gpuVertexBuffer);
    SDL_ReleaseGPUBuffer(graphicsSystem->gpuDevice, gpuIndexBuffer);
    SDL_ReleaseGPUBuffer(graphicsSystem->gpuDevice, gpuVertexUniformBuffer);
}
