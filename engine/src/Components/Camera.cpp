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
    orbitAngle += orbitSpeed * 0.15F;

    float radius = 30.0f;
    glm::vec3 targetPosition(0, 0, 0);
    m_transform.position = glm::vec3(
        targetPosition.x + radius * cos(orbitAngle),
        targetPosition.y + 15.0f,
        targetPosition.z + radius * sin(orbitAngle));
}

void Camera::OnDestroy()
{
}
