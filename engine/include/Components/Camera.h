#pragma once

#include <SDL3/SDL.h>
#include "Components/Component.h"

namespace MonkeyDEngine
{
    class Camera : public Component
    {
    protected:
        float orbitAngle = 0.0f, orbitSpeed = 0.015f;

    public:
        Camera();
        ~Camera() = default;

        virtual void Start() override;
        virtual void Update() override;
        virtual void Render() override;
    };
};
