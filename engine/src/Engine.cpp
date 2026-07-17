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

        g_Context.mainCamera = new Camera();

        MeshRenderer *meshToRender = new MeshRenderer();
        meshToRender->m_transform.position = glm::vec3{-2.f, 0.f, 0.f};
        meshToRender->Start();

        MeshRenderer *meshToRender2 = new MeshRenderer();
        meshToRender2->m_transform.position = glm::vec3{2.f, 0.f, 0.f};
        meshToRender2->Start();

        bool running = true;
        SDL_Event event{0};
        while (running)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_EVENT_WINDOW_RESIZED:
                    SystemLocator::Instance().Get<GraphicsSystem>()->CreateDepthTexture();
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    running = false;
                    break;
                default:
                    break;
                }
            }

            // Game Update
            if (g_Context.mainCamera)
                g_Context.mainCamera->Update();
            meshToRender->Update();
            meshToRender2->Update();

            // Graphics Render
            SystemLocator::Instance().Get<GraphicsSystem>()->Render3D();
            // End Rendering
        }

        // Destroy Resources
        meshToRender->OnDestroy();
        meshToRender2->OnDestroy();

        SystemLocator::Instance().Reset();

        SDL_DestroyWindow(g_Context.window);
        SDL_Quit();

        return SDL_APP_CONTINUE;
    }
}
