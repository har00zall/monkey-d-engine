#include "Core/System/SystemManager.h"
#include "Systems/GraphicsSystem.h"
#include "Context.h"
#include "Shader.h"

using namespace MonkeyDEngine;

MonkeyDEngine::Shader::Shader()
{
}

Shader::~Shader()
{
}

Shader::Shader(
    std::string shaderFilePath,
    SDL_GPUShaderFormat shaderFormat,
    SDL_GPUShaderStage shaderStage,
    Uint32 samplerCount,
    Uint32 storageBuffersCount,
    Uint32 storageTexturesCount,
    Uint32 uniformBuffersCount)
{
    m_shaderFilePath = shaderFilePath;
    m_shaderFormat = shaderFormat;
    m_shaderStage = shaderStage;
    m_samplerCount = samplerCount;
    m_storageBuffersCount = storageBuffersCount;
    m_storageTexturesCount = storageTexturesCount;
    m_uniformBuffersCount = uniformBuffersCount;
}

void Shader::Dispose()
{
    if (shaderInstance)
        SDL_ReleaseGPUShader(g_Context.gpuDevice, shaderInstance);
}

SDL_GPUShader *MonkeyDEngine::Shader::GetShaderInstance()
{
    if (shaderInstance)
        return shaderInstance;

    auto graphicsSystem = SystemManager::Instance().GetSystem<GraphicsSystem>();
    graphicsSystem->gpuShaderData.AddShader(this);

    return CreateShader();
}

SDL_GPUShader *Shader::CreateShader()
{
    if (shaderInstance)
        return shaderInstance;

    size_t shaderCodeSize;
    void *shaderCode = SDL_LoadFile(m_shaderFilePath.c_str(), &shaderCodeSize);
    if (!shaderCode)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Load Shader Error: %s", SDL_GetError());
    }

    SDL_GPUShaderCreateInfo shaderCreateInfo{};
    shaderCreateInfo.code = (Uint8 *)shaderCode;
    shaderCreateInfo.code_size = shaderCodeSize;
    shaderCreateInfo.entrypoint = "main";
    shaderCreateInfo.format = m_shaderFormat;
    shaderCreateInfo.num_samplers = m_samplerCount;
    shaderCreateInfo.num_storage_buffers = m_storageBuffersCount;
    shaderCreateInfo.num_storage_textures = m_storageTexturesCount;
    shaderCreateInfo.num_uniform_buffers = m_uniformBuffersCount;
    shaderCreateInfo.stage = m_shaderStage;

    shaderInstance = SDL_CreateGPUShader(g_Context.gpuDevice, &shaderCreateInfo);

    if (!shaderInstance)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Creating Shader Error: %s", SDL_GetError());
    }

    SDL_free(shaderCode);

    return shaderInstance;
}
