#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Context.h"
#include "Camera.h"
#include "Core/OS/Time.h"

using namespace MonkeyDEngine;

Camera::Camera()
{
    if (!g_Context.mainCamera)
        g_Context.mainCamera = this;
}

void Camera::Start()
{
}

void Camera::Update()
{
}

void Camera::OnDestroy()
{
}
