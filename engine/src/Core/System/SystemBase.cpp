#include <SDL3/SDL.h>
#include <type_traits>
#include "Core/System/SystemBase.h"
#include "Core/System/SystemLocator.h"

using namespace MonkeyDEngine;

void SystemBase::StartSystem()
{
    OnStartSystem();
}

void SystemBase::StopSystem()
{
    OnStopSystem();
}
