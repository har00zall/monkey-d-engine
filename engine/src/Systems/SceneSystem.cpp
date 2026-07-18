#include <vector>
#include "Components/Component.h"
#include "Systems/SceneSystem.h"

using namespace MonkeyDEngine;

void SceneSystem::RegisterScene(std::shared_ptr<Scene> sceneToRegister)
{
    m_scenes.push_back(sceneToRegister);
}

void SceneSystem::LoadScene(int sceneIndex, SceneLoadMode loadMode = SceneLoadMode::Single)
{
    if (m_scenes.empty() || sceneIndex >= m_scenes.size())
        return;

    auto sceneToLoad = m_scenes[sceneIndex];
    if (loadMode == SceneLoadMode::Single)
    {
        if (m_activeScene)
        {
            m_activeScene->Dispose();
            m_activeScene = nullptr;
        }

        m_activeScene = sceneToLoad;
        m_activeScene->Activate();
    }
    else
    {
        m_loadedScenes.push_back(sceneToLoad);

        sceneToLoad->Activate();
    }
}

void SceneSystem::UnloadScene(int sceneIndex)
{
    if (sceneIndex < 0 || sceneIndex >= static_cast<int>(m_scenes.size()))
        return;

    std::shared_ptr<Scene> sceneToUnload = m_scenes[sceneIndex];

    if (!sceneToUnload)
        return;

    if (m_activeScene == sceneToUnload)
        return;

    auto it = std::find(m_loadedScenes.begin(), m_loadedScenes.end(), sceneToUnload);
    if (it == m_loadedScenes.end())
        return;

    m_loadedScenes.erase(it);
    sceneToUnload->Dispose();
}

void SceneSystem::Update()
{
    if (!m_activeScene)
        return;

    m_activeScene->Update();

    if (m_loadedScenes.empty())
        return;

    for (auto &scene : m_loadedScenes)
        scene->Update();
}

void SceneSystem::OnStartSystem()
{
    if (m_scenes.empty())
        return;

    LoadScene(0);
}

void SceneSystem::OnStopSystem()
{
    if (m_activeScene)
    {
        m_activeScene->Dispose();

        m_scenes.erase(
            std::remove(m_scenes.begin(), m_scenes.end(), m_activeScene),
            m_scenes.end());
        m_activeScene = nullptr;
    }

    if (!m_loadedScenes.empty())
    {
        for (auto &scene : m_loadedScenes)
        {
            scene->Dispose();
        }

        m_loadedScenes.clear();
    }
}

void Scene::Activate()
{
    if (entities.empty())
        return;

    for (auto &entity : entities)
    {
        entity->Start();
    }
}

void Scene::Update()
{
    if (entities.empty())
        return;

    for (auto &entity : entities)
    {
        entity->Update();
    }
}

void Scene::Dispose()
{
    if (entities.empty())
        return;

    for (auto &entity : entities)
    {
        entity->OnDestroy();
    }

    entities.clear();
}

void Entity::Start()
{
    if (components.empty())
        return;

    for (auto &component : components)
    {
        component->Start();
    }
}

void Entity::Update()
{
    if (components.empty())
        return;

    for (auto &component : components)
    {
        component->Update();
    }
}

void Entity::OnDestroy()
{
    if (components.empty())
        return;

    for (auto &component : components)
    {
        component->OnDestroy();
    }
}
