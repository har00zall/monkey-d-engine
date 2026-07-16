#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace MonkeyDEngine
{
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
};
