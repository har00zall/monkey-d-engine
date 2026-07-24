#pragma once

#include <SDL3/SDL.h>
#include "Core/3D/Geometry.h"

namespace MonkeyDEngine
{
    class Entity;
    class Component
    {
    public:
        Component() = default;
        virtual ~Component() = default;

        Entity *owner;
        Transform transform{};

        virtual void Start();
        virtual void Update();
        virtual void OnDestroy();
        virtual void OnEvent(SDL_Event *event);
    };
};
