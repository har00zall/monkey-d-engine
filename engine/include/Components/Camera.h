#pragma once

#include <SDL3/SDL.h>
#include "Components/Component.h"

namespace MonkeyDEngine
{
    class Camera : public Component
    {
    public:
        Camera();
        ~Camera() = default;

        void Start() override;
        void Update() override;
        void OnDestroy() override;
    };
};
