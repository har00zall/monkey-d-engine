#include <SDL3/SDL.h>
#include "Component.h"

using namespace MonkeyDEngine;

void Component::OnStart()
{
}

void Component::OnUpdate()
{
}

void Component::Start()
{
    if (!enabled)
        return;

    OnStart();
}

void Component::Update()
{
    if (!enabled)
        return;

    OnUpdate();
}

void Component::OnDestroy()
{
}

void Component::OnEvent(SDL_Event *event)
{
}
