#include <SDL3/SDL.h>

namespace MonkeyDEngine
{
    class Camera;
    struct Size
    {
        Uint32 width;
        Uint32 height;
    };

    struct Context
    {
        SDL_Window *window;
        SDL_GPUDevice *gpuDevice;

        Camera *mainCamera;

        Size swapchainTextureSize{};
    };

    inline Context g_Context{};
}
