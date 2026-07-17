#include <SDL3/SDL.h>
#include "Components/Camera.h"

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
        Camera *mainCamera;

        Size swapchainTextureSize{};
    };

    inline Context g_Context{};
}
