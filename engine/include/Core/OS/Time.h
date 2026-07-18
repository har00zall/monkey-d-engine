#pragma once

#include <SDL3/SDL.h>

namespace MonkeyDEngine
{
    struct Time
    {
        inline static Uint64 ticks;
        inline static float deltaTime;
    };
};
