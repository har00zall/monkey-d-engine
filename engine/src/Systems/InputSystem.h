#pragma once

#include "Core/System/SystemBase.h"

namespace MonkeyDEngine
{
    struct MouseEvent
    {
        float deltaX;
        float deltaY;
    };

    class InputSystem : public SystemBase
    {
    public:
        inline static MouseEvent *mouse = new MouseEvent();

    public:
        void HandlerInputEvent(const SDL_Event *event);

        void OnUpdateSystem();

    protected:
        void OnStartSystem() override;
        void OnStopSystem() override;
    };
};
