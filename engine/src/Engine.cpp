#include <vector>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Core/System/SystemBase.h"
#include "Core/System/SystemManager.h"
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

        // Init Engine System
        SDL_Log("[Starting] Register and Start Systems");
        SystemManager::Instance()
            .RegisterSystems({
                {.autoStart = true, .type = typeid(GraphicsSystem), .instance = std::make_shared<GraphicsSystem>()},
            });
        SDL_Log("[End] Systems Registered and Started");

        g_Context.mainCamera = new Camera();
        float startingX = -3, startingZ = 3;
        std::vector<MeshRenderer *> meshesToRender;
        for (Uint16 x = 0; x < 2; x++)
        {
            for (Uint16 z = 0; z < 1; z++)
            {
                MeshRenderer *meshToRender = new MeshRenderer("assets/monkey_chad.gltf", "assets/monkey_diffuse.png");
                meshToRender->m_transform.position = glm::vec3{startingX + x * 6.f, 0.f, startingZ - z * 6.f};
                // meshToRender->m_transform.scale = glm::vec3(0.5f);
                meshToRender->Start();

                meshesToRender.push_back(meshToRender);
            }
        }

        bool running = true;
        SDL_Event event{0};
        while (running)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_EVENT_WINDOW_RESIZED:
                    SystemManager::Instance().GetSystem<GraphicsSystem>()->CreateDepthTexture();
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
            for (auto meshToRender : meshesToRender)
                meshToRender->Update();

            // Graphics Render
            SystemManager::Instance().GetSystem<GraphicsSystem>()->Render3D();
            // End Rendering
        }

        // Destroy Resources
        for (auto meshToRender : meshesToRender)
            meshToRender->OnDestroy();

        SystemManager::Instance().Dispose();

        SDL_DestroyWindow(g_Context.window);
        SDL_Quit();

        return SDL_APP_CONTINUE;
    }
}
