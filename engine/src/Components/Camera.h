#pragma once

#include <SDL3/SDL.h>
#include "Components/Component.h"

namespace MonkeyDEngine
{
    class Camera : public Component
    {
    protected:
        void OnStart() override;
        void OnUpdate() override;

    public:
        Camera();
        ~Camera() = default;
        void OnDestroy() override;
    };
};
