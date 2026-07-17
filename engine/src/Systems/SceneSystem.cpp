#include <vector>
#include "Components/Component.h"
#include "Systems/SceneSystem.h"

using namespace MonkeyDEngine;

void SceneSystem::RegisterScene(std::shared_ptr<Scene> sceneToRegister)
{
    m_scenes.push_back(sceneToRegister);

    if (!m_activeScene)
    {
        m_activeScene = std::move(sceneToRegister);
    }
}

void SceneSystem::Update()
{
    if (!m_activeScene)
        return;

    m_activeScene->Update();
}

void SceneSystem::OnStartSystem()
{
    // if (!m_activeScene)
    // {
    //     m_activeScene = m_scenes[0];
    // }
}

void SceneSystem::OnStopSystem()
{
    if (m_activeScene)
    {
        for (auto &entityInActiveScene : m_activeScene->entities)
        {
            entityInActiveScene->OnDestroy();
        }

        m_scenes.erase(
            std::remove(m_scenes.begin(), m_scenes.end(), m_activeScene),
            m_scenes.end());
        m_activeScene = nullptr;
    }

    if (!m_scenes.empty())
    {
        for (auto &scene : m_scenes)
        {
            for (auto &entityInActiveScene : scene->entities)
            {
                entityInActiveScene->OnDestroy();
            }
        }

        m_scenes.clear();
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
