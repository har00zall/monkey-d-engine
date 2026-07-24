#include <SDL3/SDL.h>
#include <exception>
#include <vector>
#include "Context.h"
#include "Core/3D/Shader.h"
#include "Core/3D/Geometry.h"
#include "Components/Renderer.h"
#include "Components/MeshRenderer.h"
#include "Components/Camera.h"
#include "../../../vendors/imgui/imgui.h"
#include "../../../vendors/imgui/imgui_impl_sdl3.h"
#include "../../../vendors/imgui/imgui_impl_sdlgpu3.h"
#include "Engine.h"
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

    ImGui_ImplSDL3_InitForSDLGPU(g_Context.window);
    ImGui_ImplSDLGPU3_InitInfo initInfo{};
    initInfo.Device = g_Context.gpuDevice;
    initInfo.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(g_Context.gpuDevice, g_Context.window);
    initInfo.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    initInfo.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
    initInfo.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
    ImGui_ImplSDLGPU3_Init(&initInfo);

    SDL_Log("\t\t\t\t[End] Graphics System Initialization Complete");

    // Creating main camera (for now I put it here)
    // mainCamera = new Camera();
}

void GraphicsSystem::OnStopSystem()
{
    SDL_WaitForGPUIdle(g_Context.gpuDevice);
    SDL_Log("\t\t\t[Starting] Clearing Graphics System");
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui::DestroyContext();

    if (depthTexture)
        SDL_ReleaseGPUTexture(g_Context.gpuDevice, depthTexture);

    // destroy the GPU device
    SDL_DestroyGPUDevice(g_Context.gpuDevice);
    SDL_Log("\t\t\t[End] Graphics System Clearing Completed");
}

int GraphicsSystem::Render3D()
{
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (USE_IMGUI_DEBUGGER)
    {
        if (ImGui::Begin("Metric", 0))
        {
            ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("%d vertices, %d indices (%d triangles)", ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);
            // ImGui::Text("%d allocations", (int)GImAllocatorActiveAllocationsCount);
        }
        ImGui::End();
        ImGui::ShowDebugLogWindow();
    }

    ImGui::Render();

    ImDrawData *drawData = ImGui::GetDrawData();

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
    RenderContext renderContext{};
    renderContext.renderPass = SDL_BeginGPURenderPass(gpuCommandBuffer, &colorTargetInfo, 1, &depthTargetInfo);
    for (auto &rendererToRender : MeshRenderer::m_meshBufferDataMap)
    {

        renderContext.vertexUniformBufferObject.viewProjection = g_Context.mainCamera->transform.GetViewProjectionMatrix();

        // draw calls go here
        for (auto meshToRender : rendererToRender.second->renderers)
        {
            if (renderContext.currentModelIndex >= MAX_INSTANCE)
                break;
            meshToRender->Render(renderContext);

            renderContext.vertexUniformBufferObject.model[renderContext.currentModelIndex] = meshToRender->transform.GetModelMatrix();

            renderContext.currentModelIndex++;
        }

        // SDL_Log("Push GPU Vertex Uniform Data");
        SDL_PushGPUVertexUniformData(gpuCommandBuffer, 0, &renderContext.vertexUniformBufferObject, sizeof(VertexUniformBufferObject));

        // SDL_Log("Draw Index");
        SDL_DrawGPUIndexedPrimitives(renderContext.renderPass, rendererToRender.second->mesh.GetIndexCount(), renderContext.currentModelIndex, 0, 0, 0);
    }
    SDL_EndGPURenderPass(renderContext.renderPass); // end the render pass

    // TO DO: Create separate pipeline for UI
    ImGui_ImplSDLGPU3_PrepareDrawData(drawData, gpuCommandBuffer);

    // create the color target (now +depth)
    SDL_GPUColorTargetInfo uiColorTargetInfo{};
    uiColorTargetInfo.clear_color = {186.f / 255.f, 221.f / 255.f, 250.f / 250.f, 1.0f};
    uiColorTargetInfo.load_op = SDL_GPU_LOADOP_LOAD;
    uiColorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    uiColorTargetInfo.texture = swapchainTexture;
    uiColorTargetInfo.layer_or_depth_plane = 0;

    SDL_GPURenderPass *uiRenderPass = SDL_BeginGPURenderPass(gpuCommandBuffer, &uiColorTargetInfo, 1, NULL);
    ImGui_ImplSDLGPU3_RenderDrawData(drawData, gpuCommandBuffer, uiRenderPass);

    SDL_EndGPURenderPass(uiRenderPass);

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
