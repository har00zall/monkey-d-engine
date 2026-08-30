#pragma once

#include "Core/TransformTypeDef.h"
#include "Component.h"

namespace MonkeyDEngine
{
    class Transform : public Component, public std::enable_shared_from_this<Transform>
    {
    protected:
        Vector3 position{};
        Vector3 rotation{};
        Vector3 scale{1.0f, 1.0f, 1.0f};

        Vector3 up;
        Vector3 right;
        Vector3 forward;

    public:
        Transform() = default;
        ~Transform() = default;

        std::weak_ptr<Transform> m_parent;
        std::vector<std::shared_ptr<Transform>> m_children;

        Vector3 GetPosition() const { return position; }
        Vector3 GetRotation() const { return rotation; }
        Vector3 GetScale() const { return scale; }

        void SetPosition(Vector3 newPosition)
        {
            position = {newPosition.x, newPosition.y, -newPosition.z};
            if (m_children.size() > 0)
            {
                for (auto childTransform : m_children)
                {
                    childTransform->Translate(position);
                }
            }
            CalculateDirection();
        }
        void SetRotation(Vector3 newRotation)
        {
            rotation = newRotation;
            if (m_children.size() > 0)
            {
                for (auto childTransform : m_children)
                {
                    childTransform->Rotate(rotation);
                }
            }
            CalculateDirection();
        }
        void SetScale(Vector3 newScale) { scale = newScale; }

        Vector3 GetForward() const { return forward; }
        Vector3 GetRight() const { return right; }
        Vector3 GetUp() const { return up; }

        glm::mat4 GetModelMatrix();
        glm::mat4 GetViewProjectionMatrix();

        void SetParent(std::shared_ptr<Transform> parent);

        void Translate(Vector3 delta);
        void Rotate(Vector3 delta);
        void Scale(Vector3 delta);

        void CalculateDirection();
        void LookAt(Vector3 target);
    };
};
