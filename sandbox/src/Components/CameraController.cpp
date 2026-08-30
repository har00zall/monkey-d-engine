#include "Context.h"
#include "Core/OS/Time.h"
#include "Core/3D/Geometry.h"
#include "Core/TransformTypeDef.h"
#include "Systems/InputSystem.h"
#include "Systems/SceneSystem.h"
#include "Components/Camera.h"
#include "Components/CameraController.h"
#include "Components/CharacterController.h"

using namespace MonkeyDEngine;

void CameraController::OnStart()
{
    SDL_SetWindowRelativeMouseMode(g_Context.window, true);
    SDL_SetWindowMouseGrab(g_Context.window, true);
}

void CameraController::OnUpdate()
{
    float currentMouseX, currentMouseY;
    auto keyStates = SDL_GetKeyboardState(NULL);
    auto mouseStates = SDL_GetMouseState(&currentMouseX, &currentMouseY);

    int directionH = 0;
    int directionF = 0;
    int directionV = 0;

    if (keyStates[SDL_SCANCODE_A])
        directionH = 1;
    if (keyStates[SDL_SCANCODE_D])
        directionH = -1;
    if (keyStates[SDL_SCANCODE_W])
        directionF = 1;
    if (keyStates[SDL_SCANCODE_S])
        directionF = -1;
    if (keyStates[SDL_SCANCODE_E])
        directionV = 1;
    if (keyStates[SDL_SCANCODE_Q])
        directionV = -1;

    Vector3 motion = (directionH * movementSpeed * Time::deltaTime * -owner->GetTransform()->GetRight()) +
                     (directionF * movementSpeed * Time::deltaTime * owner->GetTransform()->GetForward()) +
                     (0 * Time::deltaTime * owner->GetTransform()->GetUp());
    // m_characterController->Move({motion.x, motion.y, motion.z});
    // SDL_Log("Player motion: %f, %f, %f", motion.x, motion.y, motion.z);
    owner->GetTransform()->Translate(motion);

    owner->GetTransform()
        ->Rotate({0.f, InputSystem::mouse->deltaX * lookSpeed * Time::deltaTime, 0.f});
    g_Context.mainCamera->owner->GetTransform()->Rotate({InputSystem::mouse->deltaY * -lookSpeed * Time::deltaTime, 0.f, 0.f});

    SDL_Log("Player position: %f, %f, %f", owner->GetTransform()->GetPosition().x, owner->GetTransform()->GetPosition().y, owner->GetTransform()->GetPosition().z);
    // SDL_Log("Player rotation: %f, %f, %f", owner->GetTransform()->GetRotation().x, owner->GetTransform()->GetRotation().y, owner->GetTransform()->GetRotation().z);
}
