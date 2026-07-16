#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Core/System/SystemBase.h"
#include "Core/System/SystemLocator.h"
#include "Systems/GraphicsSystem.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Context.h"
#include "Engine.h"

namespace MonkeyDEngine
{
    int Engine::Run()
    {
        // Init Subsystem Video
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialized: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }
        SDL_Log("System has been initialized");

        // Init Engine System
        SDL_Log("[Starting] Creating Systems");
        SystemLocator::Instance().Provide<GraphicsSystem>(
            std::make_shared<GraphicsSystem>());
        SDL_Log("[End] Systems Created");

        // Init Window
        g_Context.window = SDL_CreateWindow(
            "SDL3 Hello World",
            1280, 720,
            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (!g_Context.window)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s", SDL_GetError());
            return SDL_APP_FAILURE;
        }
        SDL_Log("Window has been created");

        SDL_Log("[Starting] Starting Systems");
        if (SystemLocator::Instance().Has<GraphicsSystem>())
            SystemLocator::Instance().Get<GraphicsSystem>()->StartSystem();
        SDL_Log("[End] Systems Started");

        MeshRenderer *meshToRender = new MeshRenderer();
        meshToRender->GetTransform().position = glm::vec3{0.0f};
        meshToRender->Start();

        bool running = true;
        SDL_Event event{0};
        while (running)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    running = false;
                    break;
                default:
                    break;
                }
            }

            // Game Update
            meshToRender->Update();

            // Graphics Render
            SDL_Log("[Started] Rendering 3D");
            // acquire the command buffer
            SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(SystemLocator::Instance().Get<GraphicsSystem>()->gpuDevice);

            // get the swapchain texture
            SDL_GPUTexture *swapchainTexture;
            SDL_WaitAndAcquireGPUSwapchainTexture(
                commandBuffer,
                g_Context.window, &swapchainTexture,
                &g_Context.swapchainTextureSize.width,
                &g_Context.swapchainTextureSize.height);

            // end the frame early if a swapchain texture is not available
            if (swapchainTexture == NULL)
            {
                SDL_Log("Swapchain texture is null");
                // you must always submit the command buffer
                SDL_SubmitGPUCommandBuffer(commandBuffer);
                return SDL_APP_CONTINUE;
            }

            SDL_Log("[Started] Prerender Process Mesh");
            meshToRender->Render(commandBuffer);
            SDL_Log("[End] Prerender Process Mesh");

            // create the color target (now +depth)
            SDL_Log("[Started] Render Pass Creation");
            SDL_GPUColorTargetInfo colorTargetInfo{};
            colorTargetInfo.clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
            colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
            colorTargetInfo.texture = swapchainTexture;

            SDL_GPUDepthStencilTargetInfo depthTargetInfo{};
            depthTargetInfo.clear_depth = 1.0f;
            depthTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            depthTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
            depthTargetInfo.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
            depthTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
            depthTargetInfo.texture = SystemLocator::Instance().Get<GraphicsSystem>()->depthTexture;
            depthTargetInfo.cycle = true;

            // begin a render pass
            SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, &depthTargetInfo);
            SDL_Log("[End] Render Pass Creation");

            // draw calls go here
            SDL_Log("[Started] Binding Pipeline");
            SDL_BindGPUGraphicsPipeline(renderPass, SystemLocator::Instance().Get<GraphicsSystem>()->gpuGraphicsPipeline);

            // binding vertex buffer
            SDL_GPUBufferBinding vertexBufferBindings[1];
            vertexBufferBindings[0].buffer = SystemLocator::Instance().Get<GraphicsSystem>()->gpuVertexBuffer;
            vertexBufferBindings[0].offset = 0;
            SDL_Log("[Started] Binding Vertex Buffer");
            SDL_BindGPUVertexBuffers(renderPass, 0, vertexBufferBindings, 1);

            // binding index buffer
            SDL_GPUBufferBinding indexBufferBindings;
            indexBufferBindings.buffer = SystemLocator::Instance().Get<GraphicsSystem>()->gpuIndexBuffer;
            indexBufferBindings.offset = 0;
            SDL_Log("[Started] Binding Index Buffer");
            SDL_BindGPUIndexBuffer(renderPass, &indexBufferBindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);

            SDL_Log("[End] Binding Vertex Storage Buffer");

            // issue a draw call
            // SDL_DrawGPUIndexedPrimitives(renderPass, app.indexCount, 1, 0, 0, 0);
            meshToRender->PostRender(renderPass, 1);

            SDL_EndGPURenderPass(renderPass); // end the render pass
            SDL_SubmitGPUCommandBuffer(commandBuffer);
            // End Rendering
        }

        SystemLocator::Instance().Reset();

        SDL_DestroyWindow(g_Context.window);
        SDL_Quit();

        return SDL_APP_CONTINUE;
    }
}
