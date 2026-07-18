#include <SDL3/SDL.h>
#include <type_traits>
#include "SystemBase.h"
#include "SystemManager.h"

using namespace MonkeyDEngine;

void SystemBase::StartSystem()
{
    OnStartSystem();
}

void SystemBase::StopSystem()
{
    OnStopSystem();
}
