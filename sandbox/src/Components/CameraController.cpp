#include "Context.h"
#include "Core/OS/Time.h"
#include "Core/3D/Geometry.h"
#include "Components/CameraController.h"

using namespace MonkeyDEngine;

void CameraController::Start()
{
    SDL_SetWindowMouseGrab(g_Context.window, true);

    g_Context.mainCamera->transform.SetPosition({0.0f, 20, -25.0f});
    g_Context.mainCamera->transform.LookAt({0.f, 0.f, 0.f});
}

void CameraController::Update()
{
    float currentMouseX, currentMouseY;
    auto keyStates = SDL_GetKeyboardState(NULL);
    auto mouseStates = SDL_GetMouseState(&currentMouseX, &currentMouseY);
    deltaMousePositionX = currentMouseX - previousX;
    deltaMousePositionY = currentMouseY - previousY;

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

    g_Context.mainCamera->transform.Translate(
        (directionH * movementSpeed * Time::deltaTime * -g_Context.mainCamera->transform.GetRight()) +
        (directionF * movementSpeed * Time::deltaTime * g_Context.mainCamera->transform.GetForward()) +
        (directionV * movementSpeed * Time::deltaTime * g_Context.mainCamera->transform.GetUp()));

    if (mouseStates == SDL_BUTTON_LEFT)
    {
        g_Context.mainCamera->transform.Rotate({0.f, deltaMousePositionX * lookSpeed * Time::deltaTime, 0.f});
        g_Context.mainCamera->transform.Rotate({deltaMousePositionY * -lookSpeed * Time::deltaTime, 0.f, 0.f});
    }

    previousX = currentMouseX;
    previousY = currentMouseY;
}
