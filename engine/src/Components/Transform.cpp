#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Context.h"
#include "Transform.h"

using namespace MonkeyDEngine;

glm::mat4 Transform::GetModelMatrix()
{
    glm::mat4 objectModel = glm::mat4(1.0f);

    objectModel = glm::scale(objectModel, scale);
    objectModel = glm::rotate(objectModel, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.f));
    objectModel = glm::rotate(objectModel, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.f));
    objectModel = glm::rotate(objectModel, glm::radians(rotation.z), glm::vec3(0.0f, 0.f, 1.0f));
    objectModel = glm::translate(objectModel, position);

    return !m_parent.expired() ? m_parent.lock()->GetModelMatrix() * objectModel : objectModel;
}

glm::mat4 Transform::GetViewProjectionMatrix()
{
    glm::mat4 view = glm::lookAt(position, position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)g_Context.swapchainTextureSize.width / (float)g_Context.swapchainTextureSize.height, 0.1f, 100.0f);

    // converting OpenGL (glm default) axis direction into Vulkan axis direction
    // projection[1][1] *= -1;

    return projection * view;
}

void Transform::SetParent(std::shared_ptr<Transform> parent)
{
    this->m_parent = parent;
    parent->m_children.push_back(shared_from_this());
    CalculateDirection();
}

void Transform::Translate(Vector3 delta)
{
    position += delta;

    if (m_children.size() > 0)
    {
        for (auto childTransform : m_children)
        {
            childTransform->Translate(delta);
        }
    }

    CalculateDirection();
}

void Transform::Rotate(Vector3 delta)
{
    rotation += delta;
    if (m_children.size() > 0)
    {
        for (auto childTransform : m_children)
        {
            childTransform->Rotate(delta);
        }
    }
    CalculateDirection();
}

void Transform::Scale(Vector3 delta)
{
    scale += delta;
    if (m_children.size() > 0)
    {
        for (auto childTransform : m_children)
        {
            childTransform->Scale(delta);
        }
    }
}

void Transform::CalculateDirection()
{
    glm::vec3 updatedForward;
    updatedForward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
    updatedForward.y = sin(glm::radians(rotation.x));
    updatedForward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

    forward = glm::normalize(updatedForward);
    right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, forward));
}

void Transform::LookAt(Vector3 target)
{
    glm::vec3 direction = glm::normalize(target - position);
    Vector3 prevRotation = rotation;

    rotation.x = glm::degrees(std::asin(direction.y));
    rotation.y = glm::degrees(std::atan2(direction.z, direction.x));
    rotation.z = 0;

    if (m_children.size() > 0)
    {
        for (auto childTransform : m_children)
        {
            childTransform->Rotate(rotation - prevRotation);
        }
    }

    CalculateDirection();
}
