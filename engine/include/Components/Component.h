#pragma once

#include <SDL3/SDL.h>
#include "Core/3D/Geometry.h"

namespace MonkeyDEngine
{
    class Component
    {
    public:
        Transform m_transform{};
        Transform GetTransform() const { return m_transform; }

        virtual void Start();
        virtual void Update();
        virtual void Render();
        virtual void OnDestroy();
    };
};
