#pragma once

#include <string>
#include <SDL3/SDL.h>

namespace MonkeyDEngine
{
    class Material
    {
    protected:
        SDL_GPUTexture *m_texture;
        SDL_GPUSampler *m_textureSampler;

        SDL_GPUShader *vertexShader;
        SDL_GPUShader *fragmentShader;

        SDL_GPUGraphicsPipeline *gpuGraphicsPipeline;

        std::string m_vertexShaderPath;
        std::string m_fragmentShaderPath;
        std::string m_texturePath;

    public:
        Material() = default;
        Material(std::string vertexShaderPath, std::string fragmentShaderPath, std::string texturePath);
        ~Material();

        std::string shaderFilePath;
        std::string textureFilePath;

        Material *Setup();
        void Bind(SDL_GPURenderPass *renderPass);

        SDL_GPUShader *CreateShader(
            std::string shaderFilePath,
            SDL_GPUShaderFormat shaderFormat,
            SDL_GPUShaderStage shaderStage,
            Uint32 num_sampler = 0,
            Uint32 num_storage_buffers = 0,
            Uint32 num_storage_textures = 0,
            Uint32 num_uniform_buffers = 0);
    };
};
