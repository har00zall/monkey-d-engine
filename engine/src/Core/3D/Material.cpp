#include <string>
#include "Core/System/SystemManager.h"
#include "Systems/GraphicsSystem.h"
#include "Context.h"
#include "Geometry.h"
#include "Material.h"

using namespace MonkeyDEngine;

Material::Material(std::string vertexShaderPath, std::string fragmentShaderPath, std::string texturePath)
{
    m_vertexShaderPath = vertexShaderPath;
    m_fragmentShaderPath = fragmentShaderPath;
    m_texturePath = texturePath;
}

MonkeyDEngine::Material::~Material()
{
    SDL_ReleaseGPUSampler(g_Context.gpuDevice, m_textureSampler);
    SDL_ReleaseGPUTexture(g_Context.gpuDevice, m_texture);

    // release the pipeline
    SDL_ReleaseGPUGraphicsPipeline(g_Context.gpuDevice, gpuGraphicsPipeline);
}

Material *MonkeyDEngine::Material::Setup()
{
    // create gpu shader
    vertexShader = CreateShader(m_vertexShaderPath, SDL_GPU_SHADERFORMAT_SPIRV, SDL_GPU_SHADERSTAGE_VERTEX, 0, 0, 0, 1);
    fragmentShader = CreateShader(m_fragmentShaderPath, SDL_GPU_SHADERFORMAT_SPIRV, SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, 0, 1);

    // describe the vertex buffers
    SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
    vertexBufferDesctiptions[0].slot = 0;
    vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesctiptions[0].instance_step_rate = 0;
    vertexBufferDesctiptions[0].pitch = sizeof(Vertex);

    // describe the vertex attribute
    SDL_GPUVertexAttribute vertexAttributes[3];
    // inPosition
    vertexAttributes[0].buffer_slot = 0;
    vertexAttributes[0].location = 0;
    vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertexAttributes[0].offset = 0;
    // inNormal
    vertexAttributes[1].buffer_slot = 0;
    vertexAttributes[1].location = 1;
    vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertexAttributes[1].offset = sizeof(float) * 3;
    // in UV
    vertexAttributes[2].buffer_slot = 0;
    vertexAttributes[2].location = 2;
    vertexAttributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertexAttributes[2].offset = sizeof(float) * 6;

    // describe the color target
    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0] = {};
    colorTargetDescriptions[0].blend_state.enable_blend = true;
    colorTargetDescriptions[0].blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(g_Context.gpuDevice, g_Context.window);

    // create the graphics pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_FRONT;
    pipelineInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_CLOCKWISE;
    pipelineInfo.depth_stencil_state.enable_depth_test = true;
    pipelineInfo.depth_stencil_state.enable_depth_write = true;
    pipelineInfo.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;
    pipelineInfo.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    pipelineInfo.target_info.has_depth_stencil_target = true;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 3;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

    gpuGraphicsPipeline = SDL_CreateGPUGraphicsPipeline(g_Context.gpuDevice, &pipelineInfo);

    // we don't need to store the shaders after creating the pipeline
    SDL_ReleaseGPUShader(g_Context.gpuDevice, vertexShader);
    SDL_ReleaseGPUShader(g_Context.gpuDevice, fragmentShader);

    SDL_Surface *surface = SDL_LoadPNG(m_texturePath.c_str());
    SDL_Surface *rgbaSurface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);

    // create texture
    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.width = rgbaSurface->w;
    textureInfo.height = rgbaSurface->h;
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;
    m_texture = SDL_CreateGPUTexture(g_Context.gpuDevice, &textureInfo);
    Uint32 textureSize = rgbaSurface->pitch * rgbaSurface->h;

    // create sampler
    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    m_textureSampler = SDL_CreateGPUSampler(g_Context.gpuDevice, &samplerInfo);

    // create a transfer buffer to upload to the vertex buffer
    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size = (Uint32)(textureSize);
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer *gpuTransferBuffer = SDL_CreateGPUTransferBuffer(g_Context.gpuDevice, &transferInfo);

    // fill the transfer buffer
    Uint8 *mapped = (Uint8 *)SDL_MapGPUTransferBuffer(g_Context.gpuDevice, gpuTransferBuffer, false);

    SDL_memcpy(mapped, rgbaSurface->pixels, textureSize);
    SDL_UnmapGPUTransferBuffer(g_Context.gpuDevice, gpuTransferBuffer);

    // start a copy pass
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(g_Context.gpuDevice);
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    // data for texture
    SDL_GPUTextureTransferInfo textureTransferInfo{};
    textureTransferInfo.offset = 0;
    textureTransferInfo.transfer_buffer = gpuTransferBuffer;

    SDL_GPUTextureRegion textureTransferRegion{};
    textureTransferRegion.texture = m_texture;
    textureTransferRegion.w = rgbaSurface->w;
    textureTransferRegion.h = rgbaSurface->h;
    textureTransferRegion.d = 1;

    // upload texture data
    SDL_UploadToGPUTexture(copyPass, &textureTransferInfo, &textureTransferRegion, false);

    // end the copy pass
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    SDL_ReleaseGPUTransferBuffer(g_Context.gpuDevice, gpuTransferBuffer);

    // Clean up RAM resources
    SDL_DestroySurface(rgbaSurface);

    return this;
}

void MonkeyDEngine::Material::Bind(SDL_GPURenderPass *renderPass)
{
    SDL_BindGPUGraphicsPipeline(renderPass, gpuGraphicsPipeline);

    SDL_GPUTextureSamplerBinding textureBindingInfo{};
    textureBindingInfo.sampler = m_textureSampler;
    textureBindingInfo.texture = m_texture;
    SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBindingInfo, 1);
}

SDL_GPUShader *Material::CreateShader(
    std::string shaderFilePath,
    SDL_GPUShaderFormat shaderFormat,
    SDL_GPUShaderStage shaderStage,
    Uint32 num_sampler,
    Uint32 num_storage_buffers,
    Uint32 num_storage_textures,
    Uint32 num_uniform_buffers)
{
    size_t shaderCodeSize;
    void *shaderCode = SDL_LoadFile(shaderFilePath.c_str(), &shaderCodeSize);
    if (!shaderCode)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Load Shader Error: %s", SDL_GetError());
    }

    SDL_GPUShaderCreateInfo shaderCreateInfo{};
    shaderCreateInfo.code = (Uint8 *)shaderCode;
    shaderCreateInfo.code_size = shaderCodeSize;
    shaderCreateInfo.entrypoint = "main";
    shaderCreateInfo.format = shaderFormat;
    shaderCreateInfo.num_samplers = num_sampler;
    shaderCreateInfo.num_storage_buffers = num_storage_buffers;
    shaderCreateInfo.num_storage_textures = num_storage_textures;
    shaderCreateInfo.num_uniform_buffers = num_uniform_buffers;
    shaderCreateInfo.stage = shaderStage;

    SDL_GPUShader *shader = SDL_CreateGPUShader(g_Context.gpuDevice, &shaderCreateInfo);

    if (!shader)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Creating Shader Error: %s", SDL_GetError());
    }

    SDL_free(shaderCode);

    return shader;
}
