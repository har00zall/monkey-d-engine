
#include <SDL3/SDL.h>
#include "Core/System/SystemManager.h"
#include "Systems/GraphicsSystem.h"
#include "Components/Renderer.h"

using namespace MonkeyDEngine;

void Renderer::Start()
{
    graphicsSystem = SystemManager::Instance().GetSystem<GraphicsSystem>();
    graphicsSystem->gpuRenderPass.renderers.push_back(this);
}

void Renderer::Update()
{
}

void Renderer::Render()
{
}

void Renderer::OnDestroy()
{
    // release buffers
    SDL_ReleaseGPUBuffer(graphicsSystem->gpuDevice, gpuVertexBuffer);
    SDL_ReleaseGPUBuffer(graphicsSystem->gpuDevice, gpuIndexBuffer);
    SDL_ReleaseGPUBuffer(graphicsSystem->gpuDevice, gpuVertexUniformBuffer);
}
