#include <vector>
#include <SDL3/SDL.h>
#include <SDL3/SDL_time.h>
#include <glm/glm.hpp>
#include "Core/OS/Time.h"
#include "Core/System/SystemBase.h"
#include "Core/System/SystemManager.h"
#include "Systems/SceneSystem.h"
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
                {.autoStart = true, .type = typeid(SceneSystem), .instance = std::make_shared<SceneSystem>()},
                {.autoStart = true, .type = typeid(GraphicsSystem), .instance = std::make_shared<GraphicsSystem>()},
            });
        SDL_Log("[End] Systems Registered and Started");

        // Creating main scene
        auto mainScene = std::make_shared<Scene>();

        // Adding main camera
        auto cameraEntity = std::make_shared<Entity>();
        auto mainCameraComponent = std::make_shared<Camera>();
        cameraEntity->components.push_back(mainCameraComponent);
        mainScene->entities.push_back(cameraEntity);

        // Adding monkey objects ( 1 object = 1 draw call)
        float startingX = -3, startingZ = 0;
        for (Uint16 x = 0; x < 2; x++)
        {
            for (Uint16 z = 0; z < 1; z++)
            {
                auto entity = std::make_shared<Entity>();
                auto meshToRender = std::make_shared<MeshRenderer>("assets/monkey_chad.gltf", "assets/monkey_diffuse.png");
                meshToRender->m_transform.position = glm::vec3{startingX + x * 6.f, 0.f, startingZ - z * 6.f};
                meshToRender->Start();

                entity->components.push_back(meshToRender);
                mainScene->entities.push_back(entity);
            }
        }
        SystemManager::Instance().GetSystem<SceneSystem>()->RegisterScene(mainScene);

        bool running = true;
        Uint64 previousTime = 0;
        SDL_Event event{0};
        while (running)
        {
            Uint64 nowTime = SDL_GetTicks();
            Time::ticks = nowTime;
            Time::deltaTime = (nowTime - previousTime) / 1000.0f;

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
            SystemManager::Instance().GetSystem<SceneSystem>()->Update();

            // Graphics Render
            SystemManager::Instance().GetSystem<GraphicsSystem>()->Render3D();
            // End Rendering

            previousTime = nowTime;
        }

        SystemManager::Instance().Dispose();

        SDL_DestroyWindow(g_Context.window);
        SDL_Quit();

        return SDL_APP_CONTINUE;
    }
}
