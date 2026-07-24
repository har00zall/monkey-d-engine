#include "Context.h"
#include "Core/OS/Time.h"
#include "Core/3D/Geometry.h"
#include "Components/Camera.h"
#include "Components/CameraController.h"

using namespace MonkeyDEngine;

void CameraController::Start()
{
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

    bool isMouseLeftDown = mouseStates == SDL_BUTTON_LEFT;

    if (isMouseLeftDown)
    {
        g_Context.mainCamera->transform.Rotate({0.f, deltaMousePositionX * lookSpeed * Time::deltaTime, 0.f});
        g_Context.mainCamera->transform.Rotate({deltaMousePositionY * -lookSpeed * Time::deltaTime, 0.f, 0.f});
        SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_CENTER, "1");
        SDL_SetWindowMouseRect(g_Context.window, NULL);
    }

    // if (m_lastFrameCursorShowStatus && !isMouseLeftDown)
    // SDL_WarpMouseInWindow(g_Context.window, g_Context.swapchainTextureSize.width / 2, g_Context.swapchainTextureSize.height / 2);
    // SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_CENTER, "0");
    SDL_SetWindowRelativeMouseMode(g_Context.window, isMouseLeftDown);
    SDL_SetWindowMouseGrab(g_Context.window, isMouseLeftDown);

    previousX = currentMouseX;
    previousY = currentMouseY;

    m_lastFrameCursorShowStatus = isMouseLeftDown;
}
