#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <string>
#include <vector>

namespace MonkeyDEngine
{
    class SystemBase;
    struct SystemInstancePair
    {
        bool autoStart = true;
        std::type_index type;
        std::shared_ptr<SystemBase> instance;
    };
    class SystemManager
    {
    public:
        // Delete copy/move - this is a singleton-ish global registry
        SystemManager(const SystemManager &) = delete;
        SystemManager &operator=(const SystemManager &) = delete;

        static SystemManager &Instance();

        // Register a group of system instances
        void RegisterSystems(std::vector<SystemInstancePair> systemInstancePairs);

        // Register a system instance
        template <typename T>
        void RegisterSystem(std::shared_ptr<T> system)
        {
            static_assert(!std::is_same_v<T, void>, "System type cannot be void");
            static_assert(std::is_base_of<SystemBase, T>::value, "System must be derived from SystemBase");

            RegisterSystemInternal(std::type_index(typeid(T)), system);
        }

        // Retrieve a system
        template <typename T>
        T *GetSystem() const
        {
            static_assert(std::is_base_of<SystemBase, T>::value, "System must be derived from SystemBase");

            auto it = m_systems.find(std::type_index(typeid(T)));
            if (it == m_systems.end())
            {
                throw std::runtime_error(
                    std::string("System not registered: ") + typeid(T).name());
            }
            return static_cast<T *>(it->second.get());
        }

        // Check if a system exists
        template <typename T>
        bool Has() const
        {
            return m_systems.find(std::type_index(typeid(T))) != m_systems.end();
        }

        // Remove a system (useful for tests/teardown)
        template <typename T>
        void Remove() { m_systems.erase(std::type_index(typeid(T))); }

        // Clear everything (useful between test cases)
        void Clear() { m_systems.clear(); }

        void Dispose();
        void StartSystems();
        void StopSystems();

    private:
        SystemManager() = default;

        std::unordered_map<std::type_index, std::shared_ptr<SystemBase>> m_systems;

        // adding into systems map
        void RegisterSystemInternal(std::type_index type, std::shared_ptr<SystemBase> instance)
        {
            if (instance)
            {
                m_systems[type] = std::move(instance);
            }
        }
    };
}
