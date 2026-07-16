#include <SDL3/SDL.h>
#include <exception>
#include "Context.h"
#include "Core/3D/Geometry.h"
#include "Components/Camera.h"
#include "Systems/GraphicsSystem.h"

using namespace MonkeyDEngine;

void GraphicsSystem::OnStartSystem()
{
    if (!g_Context.window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Couldnt Create Graphics System Because g_Context.window is null");
        return; // failed to start the system;
    }

    SDL_Log("\t\t\t\t[Starting] Initializing Graphics System");

    // create GPU device
    gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, NULL);
    SDL_ClaimWindowForGPUDevice(gpuDevice, g_Context.window);

    SDL_GPUShader *vertexShader = CreateShader("shaders/base.vert.spv", SDL_GPU_SHADERFORMAT_SPIRV, SDL_GPU_SHADERSTAGE_VERTEX, 0, 0, 0, 1);
    SDL_GPUShader *fragmentShader = CreateShader("shaders/base.frag.spv", SDL_GPU_SHADERFORMAT_SPIRV, SDL_GPU_SHADERSTAGE_FRAGMENT, 0, 0, 0, 1);

    // create depth texture
    int w, h;
    SDL_GetWindowSizeInPixels(g_Context.window, &w, &h);
    SDL_GPUTextureCreateInfo depthTextureCreateInfo{};
    depthTextureCreateInfo.type = SDL_GPU_TEXTURETYPE_2D;
    depthTextureCreateInfo.format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    depthTextureCreateInfo.width = w;
    depthTextureCreateInfo.height = h;
    depthTextureCreateInfo.layer_count_or_depth = 1;
    depthTextureCreateInfo.num_levels = 1;
    depthTextureCreateInfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
    depthTexture = SDL_CreateGPUTexture(gpuDevice, &depthTextureCreateInfo);

    // describe the vertex buffers
    SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
    vertexBufferDesctiptions[0].slot = 0;
    vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesctiptions[0].instance_step_rate = 0;
    vertexBufferDesctiptions[0].pitch = sizeof(Vertex);

    // describe the vertex attribute
    SDL_GPUVertexAttribute vertexAttributes[2];
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
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(gpuDevice, g_Context.window);

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
    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

    try
    {
        // create the pipeline
        gpuGraphicsPipeline = SDL_CreateGPUGraphicsPipeline(gpuDevice, &pipelineInfo);
    }
    catch (const std::exception e)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create graphics pipeline: %s", SDL_GetError());
    }

    // we don't need to store the shaders after creating the pipeline
    SDL_ReleaseGPUShader(gpuDevice, vertexShader);
    SDL_ReleaseGPUShader(gpuDevice, fragmentShader);

    SDL_Log("\t\t\t\t[End] Graphics System Initialization Complete");

    // Creating main camera (for now I put it here)
    // mainCamera = new Camera();
}

void GraphicsSystem::OnStopSystem()
{
    SDL_Log("\t\t\t\t[Starting] Clearing Graphics System");
    // release buffers
    SDL_ReleaseGPUBuffer(gpuDevice, gpuVertexBuffer);
    SDL_ReleaseGPUBuffer(gpuDevice, gpuIndexBuffer);
    SDL_ReleaseGPUBuffer(gpuDevice, gpuVertexUniformBuffer);

    if (depthTexture)
    {
        SDL_ReleaseGPUTexture(gpuDevice, depthTexture);
    }

    // release the pipeline
    SDL_ReleaseGPUGraphicsPipeline(gpuDevice, gpuGraphicsPipeline);

    // destroy the GPU device
    SDL_DestroyGPUDevice(gpuDevice);
    SDL_Log("\t\t\t\t[End] Graphics System Clearing Completed");
}

int GraphicsSystem::Render3D()
{
    SDL_Log("[Started] Rendering 3D");

    // acquire the command buffer
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(gpuDevice);

    // get the swapchain texture
    SDL_GPUTexture *swapchainTexture;
    Uint32 width, height;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, g_Context.window, &swapchainTexture, &width, &height);

    // end the frame early if a swapchain texture is not available
    if (swapchainTexture == NULL)
    {
        SDL_Log("Swapchain texture is null");
        // you must always submit the command buffer
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        return SDL_APP_CONTINUE;
    }

    // create the color target (now +depth)
    SDL_Log("[Started] Render Pass Creation");
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture = swapchainTexture;

    SDL_GPUDepthStencilTargetInfo depthTargetInfo{};
    depthTargetInfo.clear_depth = 1.0f;
    depthTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    depthTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthTargetInfo.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depthTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthTargetInfo.texture = depthTexture;
    depthTargetInfo.cycle = true;

    // begin a render pass
    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, &depthTargetInfo);
    SDL_Log("[End] Render Pass Creation");

    // draw calls go here
    SDL_Log("[Started] Binding Pipeline");
    SDL_BindGPUGraphicsPipeline(renderPass, gpuGraphicsPipeline);

    // binding vertex buffer
    SDL_GPUBufferBinding vertexBufferBindings[1];
    vertexBufferBindings[0].buffer = gpuVertexBuffer;
    vertexBufferBindings[0].offset = 0;
    SDL_Log("[Started] Binding Vertex Buffer");
    SDL_BindGPUVertexBuffers(renderPass, 0, vertexBufferBindings, 1);

    // binding index buffer
    SDL_GPUBufferBinding indexBufferBindings;
    indexBufferBindings.buffer = gpuIndexBuffer;
    indexBufferBindings.offset = 0;
    SDL_Log("[Started] Binding Index Buffer");
    SDL_BindGPUIndexBuffer(renderPass, &indexBufferBindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    // binding Storage Buffer
    SDL_Log("[Started] Binding Vertex Storage Buffer");
    SDL_BindGPUVertexStorageBuffers(renderPass, 0, &gpuVertexUniformBuffer, 1);
    SDL_Log("[End] Binding Vertex Storage Buffer");

    // issue a draw call
    // SDL_DrawGPUIndexedPrimitives(renderPass, app.indexCount, 1, 0, 0, 0);

    SDL_EndGPURenderPass(renderPass);          // end the render pass
    SDL_SubmitGPUCommandBuffer(commandBuffer); // submit the command buffer

    SDL_Log("[End] Rendered 3D");

    return SDL_APP_CONTINUE;
}

SDL_GPUShader *GraphicsSystem::CreateShader(
    const char *shaderFilePath,
    SDL_GPUShaderFormat shaderFormat,
    SDL_GPUShaderStage shaderStage,
    Uint32 num_sampler,
    Uint32 num_storage_buffers,
    Uint32 num_storage_textures,
    Uint32 num_uniform_buffers)
{
    size_t shaderCodeSize;
    void *shaderCode = SDL_LoadFile(shaderFilePath, &shaderCodeSize);
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

    SDL_GPUShader *shader = SDL_CreateGPUShader(gpuDevice, &shaderCreateInfo);

    if (!shader)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Creating Shader Error: %s", SDL_GetError());
    }

    SDL_free(shaderCode);

    return shader;
}
