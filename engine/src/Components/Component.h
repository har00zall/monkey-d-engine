#pragma once

#include <memory>
#include <SDL3/SDL.h>

namespace MonkeyDEngine
{
    class Entity;
    class Component
    {
    public:
        Component() = default;
        virtual ~Component() = default;

        Entity *owner;

        virtual void Start();
        virtual void Update();
        virtual void OnDestroy();
        virtual void OnEvent(SDL_Event *event);
    };
};
