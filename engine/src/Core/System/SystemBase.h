#pragma once

#include <type_traits>
#include "Core/System/SystemManager.h"

namespace MonkeyDEngine
{
    class SystemBase
    {
    public:
        SystemBase() = default;
        virtual ~SystemBase() = default;

        virtual void StartSystem();
        virtual void StopSystem();

    protected:
        void virtual OnStartSystem() = 0;
        void virtual OnStopSystem() = 0;
    };
};
