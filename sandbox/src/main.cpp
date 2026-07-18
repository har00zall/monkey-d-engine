#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Engine.h"
#include "Core/3D/Geometry.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Systems/SceneSystem.h"

using namespace MonkeyDEngine;

int main(int argc, char *argv[])
{
    // Creating main scene
    auto mainScene = std::make_shared<Scene>();

    // Adding main camera
    auto cameraEntity = std::make_shared<Entity>();
    auto mainCameraComponent = std::make_shared<Camera>();
    cameraEntity->components.push_back(mainCameraComponent);
    mainScene->entities.push_back(cameraEntity);

    // Adding monkey objects ( 1 object = 1 draw call)
    float startingX = -3, startingZ = 0;
    for (Uint16 x = 0; x < 2; x++)
    {
        for (Uint16 z = 0; z < 1; z++)
        {
            auto entity = std::make_shared<Entity>();
            auto meshToRender = std::make_shared<MeshRenderer>("assets/monkey_chad.gltf", "assets/monkey_diffuse.png");
            meshToRender->m_transform.position = glm::vec3{startingX + x * 6.f, 0.f, startingZ - z * 6.f};

            entity->components.push_back(meshToRender);
            mainScene->entities.push_back(entity);
        }
    }

    EngineInitData initConfig{
        .title = "Sandbox",
        .windowWidth = 1280,
        .windowHeight = 720,
        .windowFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY,

        .scenes = {mainScene}};

    Engine engine(&initConfig);

    return engine.Run();
}
