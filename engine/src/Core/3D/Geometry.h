#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <tiny_gltf.h>

namespace MonkeyDEngine
{
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
