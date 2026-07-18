#pragma once

#include <SDL3/SDL.h>
#include "Components/Component.h"

namespace MonkeyDEngine
{
    class Camera : public Component
    {
    protected:
        float orbitAngle = 0.0f, orbitSpeed = 1.f;

    public:
        Camera();
        ~Camera() = default;

        void Start() override;
        void Update() override;
        void OnDestroy() override;
    };
};
