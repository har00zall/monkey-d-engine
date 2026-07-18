#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <tiny_gltf.h>

namespace MonkeyDEngine
{
    typedef glm::vec3 Vector3;

    class Transform
    {
    protected:
        Vector3 position{};
        Vector3 rotation{};
        Vector3 scale{1.0f, 1.0f, 1.0f};

        Vector3 up;
        Vector3 right;
        Vector3 forward;

    public:
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

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct Mesh
    {
        std::vector<Vertex> vertices;
        std::vector<Uint32> indices;
        Uint32 GetIndexCount() const { return (Uint32)indices.size(); };
    };

    class Geometry
    {
    public:
        static bool LoadGLTF(const char *filePath,
                             std::vector<Vertex> &outVertices, std::vector<Uint32> &outIndices);
    };
};
