#pragma once

#include <vector>
#include <memory>
#include <string>
#include <typeindex>
#include <concepts>
#include "Core/System/SystemBase.h"

namespace MonkeyDEngine
{
    class Component;
    class Entity
    {
    protected:
        std::vector<std::shared_ptr<Component>> m_components;

    public:
        std::string entityId;

        Entity() = default;

        template <typename... Components>
        inline static std::shared_ptr<Entity> Create()
        {
            auto entity = std::make_shared<Entity>();
            (entity->AddComponent<Components>(), ...);
            return entity;
        }

        template <typename T>
            requires std::derived_from<T, Component>
        std::shared_ptr<T> AddComponent()
        {
            auto newComponent = std::make_shared<T>();
            m_components.push_back(newComponent);

            return newComponent;
        }

        template <typename T>
            requires std::derived_from<T, Component>
        std::shared_ptr<T> GetComponent()
        {
            if (m_components.empty())
                return;

            auto it = std::find(m_components.begin(), m_components.end(), std::type_index(typeid(std::shared_ptr<T>)));
            if (it == m_components.end())
            {
                throw std::runtime_error(
                    std::string("No Component: ") + typeid(T).name());
            }
            return static_cast<T *>(it->second.get());
        }

        template <typename T>
            requires std::derived_from<T, Component>
        void RemoveComponent();

        void Start();
        void Update();
        void OnDestroy();
    };

    class Scene
    {
    public:
        std::vector<std::shared_ptr<Entity>> entities;

        void Activate();
        void Update();
        void Dispose();
    };

    enum class SceneLoadMode
    {
        Single,
        Additive
    };

    class SceneSystem : public SystemBase
    {
    public:
        static void RegisterScene(std::shared_ptr<Scene> sceneToRegister);
        static void LoadScene(int sceneIndex, SceneLoadMode loadMode);
        static void UnloadScene(int sceneIndex);

        void Update();

    protected:
        inline static std::shared_ptr<Scene> m_activeScene;
        inline static std::vector<std::shared_ptr<Scene>> m_loadedScenes;
        inline static std::vector<std::shared_ptr<Scene>> m_scenes;

        void OnStartSystem() override;
        void OnStopSystem() override;
    };
};
