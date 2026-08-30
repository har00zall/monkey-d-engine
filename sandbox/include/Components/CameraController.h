#pragma once

#include "Components/CharacterController.h"
#include "Components/Component.h"

class CameraController : public MonkeyDEngine::Component
{
protected:
    float movementSpeed = 15.f, lookSpeed = 15.f;
    bool m_lastFrameCursorShowStatus;

    void OnStart() override;
    void OnUpdate() override;
};
