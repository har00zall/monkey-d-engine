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

        void Activate();
        void Update();
        void Dispose();
    };

    typedef enum SceneLoadMode
    {
        Single,
        Additive
    } SceneLoadMode;

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
