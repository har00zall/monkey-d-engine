#include <vector>
#include <memory>
#include <typeindex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Engine.h"
#include "Core/3D/Geometry.h"
#include "Core/3D/Material.h"
#include "Core/3D/Shader.h"
#include "Components/Camera.h"
#include "Components/CameraController.h"
#include "Components/MeshRenderer.h"
#include "Systems/SceneSystem.h"

using namespace MonkeyDEngine;

int main(int argc, char *argv[])
{
    // Creating main scene
    auto mainScene = std::make_shared<Scene>();

    // Adding main camera
    auto cameraEntity = Entity::Create<Camera, CameraController>();
    mainScene->entities.push_back(cameraEntity);

    // Adding monkey objects ( 1 object = 1 draw call)
    float startingX = -12, startingZ = 0;
    std::shared_ptr<Shader> vertexShader = std::make_shared<Shader>("assets/shaders/base.vert.spv", SDL_GPU_SHADERFORMAT_SPIRV, SDL_GPU_SHADERSTAGE_VERTEX, 0, 0, 0, 1);
    std::shared_ptr<Shader> fragmentShader = std::make_shared<Shader>("assets/shaders/base.frag.spv", SDL_GPU_SHADERFORMAT_SPIRV, SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, 0, 1);
    for (Uint16 x = 0; x < 8; x++)
    {
        for (Uint16 z = 0; z < 8; z++)
        {
            auto entity = Entity::Create();
            auto meshToRender = entity->AddComponent<MeshRenderer>("assets/monkey_chad.gltf");
            auto meshMaterial = std::make_shared<Material>(vertexShader, fragmentShader, "assets/monkey_diffuse.png");

            meshToRender->SetMaterial(meshMaterial);
            meshToRender->transform.SetPosition({startingX + x * 6.f, 0.f, startingZ - z * 6.f});

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
