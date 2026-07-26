
#include "SDL3/SDL.h"
#include "InputSystem.h"

using namespace MonkeyDEngine;

void InputSystem::HandlerInputEvent(const SDL_Event *event)
{
}

void InputSystem::OnUpdateSystem()
{
    SDL_GetRelativeMouseState(&mouse->deltaX, &mouse->deltaY);
}

void InputSystem::OnStartSystem()
{
}

void InputSystem::OnStopSystem()
{
}
