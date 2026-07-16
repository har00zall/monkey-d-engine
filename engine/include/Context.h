#include <SDL3/SDL.h>
#include "Engine.h"

namespace MonkeyDEngine
{
    struct Size
    {
        Uint32 width;
        Uint32 height;
    };

    struct Context
    {
        SDL_Window *window;

        Size swapchainTextureSize{};
    };

    inline Context g_Context{};
}
