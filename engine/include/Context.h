#include <SDL3/SDL.h>
#include "Engine.h"

namespace MonkeyDEngine
{
    struct Context
    {
        SDL_Window *window;
    };

    inline Context g_Context{};
}
