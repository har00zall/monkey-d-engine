#pragma once

#include <string>
#include "SDL3/SDL.h"

namespace MonkeyDEngine
{
    class Shader
    {
    protected:
        SDL_GPUShader *shaderInstance = nullptr;

        std::string m_shaderFilePath;
        SDL_GPUShaderFormat m_shaderFormat;
        SDL_GPUShaderStage m_shaderStage;
        Uint32 m_samplerCount = 0;
        Uint32 m_storageBuffersCount = 0;
        Uint32 m_storageTexturesCount = 0;
        Uint32 m_uniformBuffersCount = 0;

    public:
        Shader();
        ~Shader();
        Shader(std::string shaderFilePath,
               SDL_GPUShaderFormat shaderFormat,
               SDL_GPUShaderStage shaderStage,
               Uint32 num_sampler = 0,
               Uint32 num_storage_buffers = 0,
               Uint32 num_storage_textures = 0,
               Uint32 num_uniform_buffers = 0);

        void Dispose();
        SDL_GPUShader *GetShaderInstance();
        SDL_GPUShader *CreateShader();
    };
};
