#include "Core/System/SystemLocator.h"
#include "Core/System/SystemBase.h"

namespace MonkeyDEngine
{
    SystemLocator &SystemLocator::Instance()
    {
        static SystemLocator instance;
        return instance;
    }

    void SystemLocator::Reset()
    {
        for (auto t_system : m_systems)
        {
            std::static_pointer_cast<SystemBase>(t_system.second)->StopSystem();
        }

        m_systems.clear();
    }
}
