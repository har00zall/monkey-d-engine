#include "Core/System/SystemManager.h"
#include "Core/System/SystemBase.h"

namespace MonkeyDEngine
{
    SystemManager &SystemManager::Instance()
    {
        static SystemManager instance;
        return instance;
    }

    void SystemManager::RegisterSystems(std::vector<SystemInstancePair> systemInstancePairs)
    {
        if (systemInstancePairs.empty())
            return;

        for (auto &systemInstancePair : systemInstancePairs)
        {
            if (!systemInstancePair.instance)
                continue;

            RegisterSystemInternal(systemInstancePair.type, systemInstancePair.instance);

            if (systemInstancePair.autoStart)
                systemInstancePair.instance->StartSystem();
        }
    }

    void SystemManager::StartSystems()
    {
        for (auto t_system : m_systems)
            std::static_pointer_cast<SystemBase>(t_system.second)->StartSystem();
    }

    void SystemManager::StopSystems()
    {
        for (auto t_system : m_systems)
            std::static_pointer_cast<SystemBase>(t_system.second)->StopSystem();
    }

    void SystemManager::Dispose()
    {
        StopSystems();
        Clear();
    }
}
