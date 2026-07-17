#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Core/System/SystemBase.h"

namespace MonkeyDEngine
{
    class Component;
    struct Entity
    {
        std::string entityId;
        std::vector<std::shared_ptr<Component>> components;

        void Start();
        void Update();
        void OnDestroy();
    };

    struct Scene
    {
        std::vector<std::shared_ptr<Entity>> entities;

        void Update();
    };

    class SceneSystem : public SystemBase
    {
    public:
        void RegisterScene(std::shared_ptr<Scene> sceneToRegister);

        void Update();

    protected:
        std::shared_ptr<Scene> m_activeScene;
        std::vector<std::shared_ptr<Scene>> m_scenes;

        void OnStartSystem() override;
        void OnStopSystem() override;
    };
};
