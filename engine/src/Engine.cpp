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
    Engine::Engine(const EngineInitData *initData)
    {
        Init(initData);
    }

    Engine::~Engine()
    {
        SystemManager::Instance().Dispose();

        SDL_DestroyWindow(g_Context.window);
        SDL_Quit();

        SDL_Log("[End] Engine Exited");
    }

    Engine *Engine::Init(const EngineInitData *initData)
    {
        // Init Subsystem Video
        if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialized: %s", SDL_GetError());
            return this;
        }
        SDL_Log("System has been initialized");

        // Init Window
        g_Context.window = SDL_CreateWindow(
            initData->title,
            initData->windowWidth, initData->windowHeight,
            initData->windowFlags);
        if (!g_Context.window)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s", SDL_GetError());
            return this;
        }
        SDL_Log("Window has been created");

        // Init Engine System
        SDL_Log("[Starting] Register and Start Systems");
        SystemManager::Instance()
            .RegisterSystems({
                {.autoStart = false, .type = typeid(SceneSystem), .instance = std::make_shared<SceneSystem>()},
                {.autoStart = true, .type = typeid(GraphicsSystem), .instance = std::make_shared<GraphicsSystem>()},
            });
        SDL_Log("[End] Systems Registered and Started");

        if (!initData->scenes.empty())
        {
            for (auto &scene : initData->scenes)
            {
                SystemManager::Instance().GetSystem<SceneSystem>()->RegisterScene(scene);
            }
        }

        return this;
    }

    int Engine::Run()
    {
        bool running = true;
        Uint64 previousTime = 0;
        SDL_Event event{0};

        SystemManager::Instance().GetSystem<SceneSystem>()->StartSystem();

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

        return SDL_APP_CONTINUE;
    }
}
