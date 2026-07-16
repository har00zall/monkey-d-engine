#pragma once

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <string>

namespace MonkeyDEngine
{
    class SystemBase;
    class SystemLocator
    {
    public:
        // Delete copy/move - this is a singleton-ish global registry
        SystemLocator(const SystemLocator &) = delete;
        SystemLocator &operator=(const SystemLocator &) = delete;

        static SystemLocator &Instance();

        // Register a system instance
        template <typename T>
        void Provide(std::shared_ptr<T> system)
        {
            static_assert(!std::is_same_v<T, void>, "System type cannot be void");
            static_assert(std::is_base_of<SystemBase, T>::value, "System must be derived from SystemBase");

            m_systems[std::type_index(typeid(T))] = std::move(system);
        }

        // Retrieve a system
        template <typename T>
        std::shared_ptr<T> Get() const
        {
            static_assert(std::is_base_of<SystemBase, T>::value, "System must be derived from SystemBase");

            auto it = m_systems.find(std::type_index(typeid(T)));
            if (it == m_systems.end())
            {
                throw std::runtime_error(
                    std::string("System not registered: ") + typeid(T).name());
            }
            return std::static_pointer_cast<T>(it->second);
        }

        // Check if a system exists
        template <typename T>
        bool Has() const
        {
            return m_systems.find(std::type_index(typeid(T))) != m_systems.end();
        }

        // Remove a system (useful for tests/teardown)
        template <typename T>
        void Remove()
        {
            m_systems.erase(std::type_index(typeid(T)));
        }

        // Clear everything (useful between test cases)
        void Reset();

    private:
        SystemLocator() = default;

        std::unordered_map<std::type_index, std::shared_ptr<SystemBase>> m_systems;
    };
}
