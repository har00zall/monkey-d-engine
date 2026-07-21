#define USE_IMGUI_DEBUGGER true

#pragma once

#include <vector>
#include <memory>
#include "SDL3/SDL.h"

namespace MonkeyDEngine
{
    class Scene;
    struct EngineInitData
    {
        const char *title;
        int windowWidth;
        int windowHeight;
        SDL_WindowFlags windowFlags;

        std::vector<std::shared_ptr<Scene>> scenes;
    };

    class Engine
    {
    public:
        Engine(const EngineInitData *initData);
        ~Engine();

        Engine *Init(const EngineInitData *initData);

        int Run();
    };
}
