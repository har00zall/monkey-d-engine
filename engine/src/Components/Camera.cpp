#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include "Components/Camera.h"

using namespace MonkeyDEngine;

Camera::Camera()
{
    m_transform.position = glm::vec3(0.0f, 2.5f, 8.0f);
}

void Camera::Start()
{
}

void Camera::Update()
{
    // OrbitCamera(0.14f, view, cameraPosition);
}

void Camera::Render(SDL_GPUCommandBuffer *commandBuffer)
{
}
