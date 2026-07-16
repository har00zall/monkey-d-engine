#pragma once

#include <SDL3/SDL.h>
#include "Components/Component.h"

namespace MonkeyDEngine
{
    class Camera : public Component
    {
    protected:
        Transform m_transform{};

        float orbitAngle = 0.0f, orbitSpeed = 0.015f;

    public:
        Camera();
        ~Camera() = default;

        Transform GetTransform() const { return m_transform; }

        virtual void Start() override;
        virtual void Update() override;
        virtual void Render() override;
    };
};
