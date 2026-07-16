#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <tiny_gltf.h>

namespace MonkeyDEngine
{
    struct Transform
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale = glm::vec3(1.0f);

        glm::mat4 GetModelMatrix();
        glm::mat4 GetViewProjectionMatrix();
    };

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
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
