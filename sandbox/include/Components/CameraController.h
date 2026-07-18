#pragma once

#include "Components/Component.h"

class CameraController : public MonkeyDEngine::Component
{
protected:
    float orbitAngle = 0.0f, orbitSpeed = 1.f;

public:
    void Start() override;
    void Update() override;
};
