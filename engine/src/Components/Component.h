#pragma once

#include <memory>
#include <SDL3/SDL.h>

namespace MonkeyDEngine
{
    class Entity;
    class Component
    {
    protected:
        virtual void OnStart();
        virtual void OnUpdate();

    public:
        Component() = default;
        virtual ~Component() = default;

        bool enabled = true;

        Entity *owner;

        void Start();
        void Update();

        virtual void OnDestroy();
        virtual void OnEvent(SDL_Event *event);
    };
};
