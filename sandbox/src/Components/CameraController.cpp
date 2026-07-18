#include "Context.h"
#include "Core/OS/Time.h"
#include "Components/CameraController.h"

using namespace MonkeyDEngine;

void CameraController::Start()
{
    transform.position = glm::vec3(0.0f, 2.5f, 8.0f);
}

void CameraController::Update()
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
    g_Context.mainCamera->transform.position = glm::vec3(
        targetPosition.x + radius * cos(orbitAngle),
        targetPosition.y + 15.0f,
        targetPosition.z + radius * sin(orbitAngle));
}
