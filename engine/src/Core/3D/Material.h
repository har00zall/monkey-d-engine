#pragma once

#include <string>
#include <SDL3/SDL.h>

namespace MonkeyDEngine
{
    class Shader;
    class Material
    {
    protected:
        SDL_GPUTexture *m_texture;
        SDL_GPUSampler *m_textureSampler;

        std::shared_ptr<Shader> vertexShader;
        std::shared_ptr<Shader> fragmentShader;

        SDL_GPUGraphicsPipeline *gpuGraphicsPipeline;

        std::string m_vertexShaderPath;
        std::string m_fragmentShaderPath;
        std::string m_texturePath;

    public:
        Material() = default;
        Material(std::shared_ptr<Shader> vertexShaderPath, std::shared_ptr<Shader> fragmentShaderPath, std::string texturePath);
        ~Material();

        std::string textureFilePath;

        Material *Setup();
        void Bind(SDL_GPURenderPass *renderPass);
    };
};
