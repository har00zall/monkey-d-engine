#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Context.h"
#include "Components/Camera.h"
#include "Core/OS/Time.h"

using namespace MonkeyDEngine;

Camera::Camera()
{
    m_transform.position = glm::vec3(0.0f, 2.5f, 8.0f);

    if (!g_Context.mainCamera)
        g_Context.mainCamera = this;
}

void Camera::Start()
{
}

void Camera::Update()
{
    const bool *keyStates = SDL_GetKeyboardState(NULL);
    int direction = 0;

    if (keyStates[SDL_SCANCODE_A])
        direction = 1;
    if (keyStates[SDL_SCANCODE_D])
        direction = -1;

    orbitAngle += direction * orbitSpeed * Time::deltaTime;

    float radius = 30.0f;
    glm::vec3 targetPosition(0, 0, 0);
    m_transform.position = glm::vec3(
        targetPosition.x + radius * cos(orbitAngle),
        targetPosition.y + 15.0f,
        targetPosition.z + radius * sin(orbitAngle));
}

void Camera::OnDestroy()
{
}
