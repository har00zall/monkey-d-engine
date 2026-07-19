#include <SDL3/SDL.h>
#include <exception>
#include "Context.h"
#include "Core/3D/Geometry.h"
#include "Components/Renderer.h"
#include "GraphicsSystem.h"

using namespace MonkeyDEngine;

void GraphicsSystem::OnStartSystem()
{
    if (!g_Context.window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldnt Create Graphics System Because g_Context.window is null");
        return; // failed to start the system;
    }

    SDL_Log("\t\t\t\t[Starting] Initializing Graphics System");

    // create GPU device
    g_Context.gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    SDL_ClaimWindowForGPUDevice(g_Context.gpuDevice, g_Context.window);

    // create depth texture
    CreateDepthTexture();

    SDL_Log("\t\t\t\t[End] Graphics System Initialization Complete");

    // Creating main camera (for now I put it here)
    // mainCamera = new Camera();
}

void GraphicsSystem::OnStopSystem()
{
    SDL_Log("\t\t\t[Starting] Clearing Graphics System");

    if (depthTexture)
        SDL_ReleaseGPUTexture(g_Context.gpuDevice, depthTexture);

    // destroy the GPU device
    SDL_DestroyGPUDevice(g_Context.gpuDevice);
    SDL_Log("\t\t\t[End] Graphics System Clearing Completed");
}

int GraphicsSystem::Render3D()
{
    // acquire the command buffer
    gpuCommandBuffer = SDL_AcquireGPUCommandBuffer(g_Context.gpuDevice);

    // get the swapchain texture
    SDL_GPUTexture *swapchainTexture;
    SDL_WaitAndAcquireGPUSwapchainTexture(
        gpuCommandBuffer,
        g_Context.window, &swapchainTexture,
        &g_Context.swapchainTextureSize.width,
        &g_Context.swapchainTextureSize.height);

    // end the frame early if a swapchain texture is not available
    if (swapchainTexture == NULL)
    {
        SDL_Log("Swapchain texture is null");
        // you must always submit the command buffer
        SDL_SubmitGPUCommandBuffer(gpuCommandBuffer);
        return SDL_APP_CONTINUE;
    }

    // create the color target (now +depth)
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = {186.f / 255.f, 221.f / 255.f, 250.f / 250.f, 1.0f};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;

    SDL_GPUDepthStencilTargetInfo depthTargetInfo{};
    depthTargetInfo.clear_depth = 1.0f;
    depthTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    depthTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthTargetInfo.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depthTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthTargetInfo.texture = depthTexture;
    depthTargetInfo.cycle = true;

    // begin a render pass
    gpuRenderPass.activeRenderPass = SDL_BeginGPURenderPass(gpuCommandBuffer, &colorTargetInfo, 1, &depthTargetInfo);

    // draw calls go here
    for (auto meshToRender : gpuRenderPass.renderers)
        meshToRender->Render();

    SDL_EndGPURenderPass(gpuRenderPass.activeRenderPass); // end the render pass
    SDL_SubmitGPUCommandBuffer(gpuCommandBuffer);

    return SDL_APP_CONTINUE;
}

void GraphicsSystem::CreateDepthTexture()
{
    if (depthTexture)
    {
        SDL_ReleaseGPUTexture(g_Context.gpuDevice, depthTexture);
        depthTexture = nullptr;
    }

    // create depth texture
    int w, h;
    SDL_GetWindowSizeInPixels(g_Context.window, &w, &h);
    SDL_GPUTextureCreateInfo depthTextureCreateInfo{};
    depthTextureCreateInfo.type = SDL_GPU_TEXTURETYPE_2D;
    depthTextureCreateInfo.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    depthTextureCreateInfo.width = w;
    depthTextureCreateInfo.height = h;
    depthTextureCreateInfo.layer_count_or_depth = 1;
    depthTextureCreateInfo.num_levels = 1;
    depthTextureCreateInfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
    depthTexture = SDL_CreateGPUTexture(g_Context.gpuDevice, &depthTextureCreateInfo);
}
