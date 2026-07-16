#pragma once

#include <SDL3/SDL.h>
#include "Core/3D/Geometry.h"

namespace MonkeyDEngine
{
    class Component
    {
    public:
        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
    };
};
