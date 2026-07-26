#pragma once

#include "Components/Component.h"

class CameraController : public MonkeyDEngine::Component
{
protected:
    float movementSpeed = 15.f, lookSpeed = 15.f;

    bool m_lastFrameCursorShowStatus;

public:
    void Start() override;
    void Update() override;
};
