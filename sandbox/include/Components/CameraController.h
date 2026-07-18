#pragma once

#include "Components/Component.h"

class CameraController : public MonkeyDEngine::Component
{
protected:
    float movementSpeed = 15.f, lookSpeed = 15.f;

    float previousX = 0,
          previousY = 0,
          deltaMousePositionX = 0,
          deltaMousePositionY = 0;

public:
    void Start() override;
    void Update() override;
};
