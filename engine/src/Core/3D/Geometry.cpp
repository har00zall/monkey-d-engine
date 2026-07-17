#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Context.h"
#include "Core/3D/Geometry.h"

using namespace MonkeyDEngine;

glm::mat4 Transform::GetModelMatrix()
{
    glm::mat4 objectModel = glm::mat4(1.0f);

    objectModel = glm::translate(objectModel, position);
    objectModel = glm::rotate(objectModel, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.f));
    objectModel = glm::rotate(objectModel, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.f));
    objectModel = glm::rotate(objectModel, glm::radians(rotation.z), glm::vec3(0.0f, 0.f, 1.0f));
    objectModel = glm::scale(objectModel, scale);

    return objectModel;
}

glm::mat4 Transform::GetViewProjectionMatrix()
{
    SDL_Log("Start Calculating View Projection Matrix");
    glm::mat4 view = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)g_Context.swapchainTextureSize.width / (float)g_Context.swapchainTextureSize.height, 0.1f, 100.0f);

    // converting OpenGL (glm default) axis direction into Vulkan axis direction
    projection[1][1] *= 1;
    SDL_Log("Camera Position: %f, %f, %f", position.x, position.y, position.z);

    return projection * view;
}

bool Geometry::LoadGLTF(const char *filePath,
                        std::vector<Vertex> &outVertices, std::vector<Uint32> &outIndices)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    if (!loader.LoadASCIIFromFile(&model, &err, &warn, filePath))
        return false;

    // grab first mesh and primitive
    const tinygltf::Mesh &mesh = model.meshes[0];
    const tinygltf::Primitive &primitive = mesh.primitives[0];

    // read indices
    const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
    const tinygltf::BufferView &indexBufferView = model.bufferViews[indexAccessor.bufferView];
    const tinygltf::Buffer &indexBuffer = model.buffers[indexBufferView.buffer];

    if (indexAccessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
    {
        SDL_Log("Unexpected index component type: %d", indexAccessor.componentType);
    }

    const uint16_t *indicesData =
        SDL_reinterpret_cast(const uint16_t *,
                             &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);

    for (size_t i = 0; i < indexAccessor.count; ++i)
    {
        outIndices.push_back(indicesData[i]);
    }

    // read position and normal
    const tinygltf::Accessor positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
    const tinygltf::BufferView positionBufferView = model.bufferViews[positionAccessor.bufferView];
    const float *positionData =
        SDL_reinterpret_cast(const float *,
                             &model.buffers[positionBufferView.buffer].data[positionBufferView.byteOffset + positionAccessor.byteOffset]);

    const tinygltf::Accessor normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
    const tinygltf::BufferView normalBufferView = model.bufferViews[normalAccessor.bufferView];
    const float *normalData =
        SDL_reinterpret_cast(const float *,
                             &model.buffers[normalBufferView.buffer].data[normalBufferView.byteOffset + normalAccessor.byteOffset]);

    const tinygltf::Accessor uvAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
    const tinygltf::BufferView uvBufferView = model.bufferViews[uvAccessor.bufferView];
    const float *uvData =
        SDL_reinterpret_cast(const float *,
                             &model.buffers[uvBufferView.buffer].data[uvBufferView.byteOffset + uvAccessor.byteOffset]);

    for (size_t i = 0; i < positionAccessor.count; ++i)
    {
        Vertex vertex;
        vertex.position = glm::vec3(positionData[i * 3 + 0], positionData[i * 3 + 1], positionData[i * 3 + 2]);
        vertex.normal = glm::vec3(normalData[i * 3 + 0], normalData[i * 3 + 1], normalData[i * 3 + 2]);
        vertex.uv = glm::vec2(uvData[i * 2 + 0], uvData[i * 2 + 1]);
        outVertices.push_back(vertex);
    }

    SDL_Log("First vertex pos: (%f, %f, %f)", outVertices[0].position.x, outVertices[0].position.y, outVertices[0].position.z);

    return true;
}
