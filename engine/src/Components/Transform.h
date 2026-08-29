#pragma once

#include <glm/glm.hpp>
#include "Component.h"

namespace MonkeyDEngine
{
    typedef glm::vec3 Vector3;
    class Transform : public Component
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

        Vector3 GetPosition() const { return position; }
        Vector3 GetRotation() const { return rotation; }
        Vector3 GetScale() const { return scale; }

        void SetPosition(Vector3 newPosition)
        {
            position = {newPosition.x, newPosition.y, -newPosition.z};
            CalculateDirection();
        }
        void SetRotation(Vector3 newRotation)
        {
            rotation = newRotation;
            CalculateDirection();
        }
        void SetScale(Vector3 newScale) { scale = newScale; }

        Vector3 GetForward() const { return forward; }
        Vector3 GetRight() const { return right; }
        Vector3 GetUp() const { return up; }

        glm::mat4 GetModelMatrix();
        glm::mat4 GetViewProjectionMatrix();

        void Translate(Vector3 delta);
        void Rotate(Vector3 delta);
        void Scale(Vector3 delta);

        void CalculateDirection();
        void LookAt(Vector3 target);
    };
};
