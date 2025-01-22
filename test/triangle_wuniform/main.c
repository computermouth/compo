// adapted from https://github.com/TheSpydog/SDL_gpu_examples

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_oldnames.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

typedef struct PositionColorVertex {
	float x, y, z;        // 12B
	Uint8 r, g, b, a;     // 4B
} PositionColorVertex;

float add[3] = {.5, .5, .5};
float sub[3] = {0, 0, 0};

int main(){

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return 1;
	}

    SDL_GPUDevice * device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        false,
        NULL
    );

    if (device == NULL) {
		SDL_Log("GPUCreateDevice failed");
		return 1;
    }

    SDL_Window * window = SDL_CreateWindow("title", 640, 480, (SDL_WindowFlags)(0));

    if (window == NULL) {
		SDL_Log("CreateWindow failed: %s", SDL_GetError());
		return 1;
    }

	if (!SDL_ClaimWindowForGPUDevice(device, window)) {
		SDL_Log("GPUClaimWindow failed");
		return 1;
	}

    SDL_GPUShaderFormat vert_format = SDL_GPU_SHADERFORMAT_SPIRV;
    size_t vert_shader_bc_sz;
    void * vert_shader_bc = SDL_LoadFile("PositionColor.vert.glsl.spv", &vert_shader_bc_sz);

    SDL_GPUShaderCreateInfo vert_shader_info = {
        .code = vert_shader_bc,
        .code_size = vert_shader_bc_sz,
        .entrypoint = "main",
        .format = vert_format,
        .stage = SDL_GPU_SHADERSTAGE_VERTEX,
        .num_samplers = 0,
        .num_uniform_buffers = 1,
        .num_storage_buffers = 0,
        .num_storage_textures = 0,
    };

    SDL_GPUShader * vert_shader = SDL_CreateGPUShader(device, &vert_shader_info);
	if (vert_shader == NULL)
	{
		SDL_Log("Failed to create vert_shader!");
		SDL_free(vert_shader_bc);
		return 1;
	}

    SDL_GPUShaderFormat frag_format = SDL_GPU_SHADERFORMAT_SPIRV;
    size_t frag_shader_bc_sz;
    void * frag_shader_bc = SDL_LoadFile("SolidColor.frag.glsl.spv", &frag_shader_bc_sz);

    SDL_GPUShaderCreateInfo frag_shader_info = {
        .code = frag_shader_bc,
        .code_size = frag_shader_bc_sz,
        .entrypoint = "main",
        .format = frag_format,
        .stage = SDL_GPU_SHADERSTAGE_VERTEX,
        .num_samplers = 0,
        .num_uniform_buffers = 1,
        .num_storage_buffers = 0,
        .num_storage_textures = 0,
    };

    SDL_GPUShader * frag_shader = SDL_CreateGPUShader(device, &frag_shader_info);
	if (vert_shader == NULL)
	{
		SDL_Log("Failed to create frag_shader!");
		SDL_free(frag_shader_bc);
		return 1;
	}

	// Create the pipeline
	SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.target_info = {
			.num_color_targets = 1,
			.color_target_descriptions = (SDL_GPUColorTargetDescription[]){{
				.format = SDL_GetGPUSwapchainTextureFormat(device, window),
			}},
		},
		// This is set up to match the vertex shader layout!
		.vertex_input_state = (SDL_GPUVertexInputState){
			.num_vertex_buffers = 1,
			.vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]){{
				.slot = 0,
				.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
				.instance_step_rate = 0,
				.pitch = sizeof(PositionColorVertex)
			}},
			.num_vertex_attributes = 3,
			.vertex_attributes = (SDL_GPUVertexAttribute[]){{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
				.location = 0,
				.offset = 0
			}, {
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
				.location = 1,
				.offset = sizeof(float) * 3
			}}
		},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.vertex_shader = vert_shader,
		.fragment_shader = frag_shader
	};

    SDL_GPUGraphicsPipeline * pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
	if (pipeline == NULL)
	{
		SDL_Log("Failed to create pipeline!");
		return -1;
	}

	SDL_ReleaseGPUShader(device, vert_shader);
	SDL_ReleaseGPUShader(device, frag_shader);

	// Create the vertex buffer
	SDL_GPUBuffer * vertex_buffer = SDL_CreateGPUBuffer(
		device,
		&(SDL_GPUBufferCreateInfo) {
			.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
			.size = sizeof(PositionColorVertex) * 3
		}
	);

	// To get data into the vertex buffer, we have to use a transfer buffer
	SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(
		device,
		&(SDL_GPUTransferBufferCreateInfo) {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = sizeof(PositionColorVertex) * 3
		}
	);

	PositionColorVertex* transferData = SDL_MapGPUTransferBuffer(
		device,
		transferBuffer,
		false
	);

	transferData[0] = (PositionColorVertex) {-0.5,-0.5,0,0x00,0x00,0x00,0xFF };
	transferData[1] = (PositionColorVertex) { 0.5,-0.5,0,0x00,0x00,0x00,0xFF };
	transferData[2] = (PositionColorVertex) { 0  , 0.5,0,0x00,0x00,0x00,0xFF };

	// Upload the transfer data to the vertex buffer
	SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

	SDL_UploadToGPUBuffer(
		copyPass,
		&(SDL_GPUTransferBufferLocation) {
			.transfer_buffer = transferBuffer,
			.offset = 0
		},
		&(SDL_GPUBufferRegion) {
			.buffer = vertex_buffer,
			.offset = 0,
			.size = sizeof(PositionColorVertex) * 3
		},
		false
	);

	SDL_EndGPUCopyPass(copyPass);
	SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
	SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    bool quit = 0;

    while (!quit) {

        // inputs
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			if (evt.type == SDL_EVENT_QUIT) {
				quit = true;
			} else if (evt.type == SDL_EVENT_KEY_UP) {
				if (evt.key.key == SDLK_Q)
					add[0] = fmodf(add[0] + .1, 1.0);
				if (evt.key.key == SDLK_W)
					add[1] = fmodf(add[1] + .1, 1.0);
				if (evt.key.key == SDLK_E)
					add[2] = fmodf(add[2] + .1, 1.0);
				if (evt.key.key == SDLK_A)
					sub[0] = fmodf(sub[0] + .1, 1.0);
				if (evt.key.key == SDLK_S)
					sub[1] = fmodf(sub[1] + .1, 1.0);
				if (evt.key.key == SDLK_D)
					sub[2] = fmodf(sub[2] + .1, 1.0);

				fprintf(stderr, "add: %.1f %.1f %.1f\n", add[0], add[1], add[2]);
				fprintf(stderr, "sub: %.1f %.1f %.1f\n", sub[0], sub[1], sub[2]);
				fprintf(stderr, "\n");
			}
        }

        SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(device);
        if (cmdbuf == NULL)
        {
            SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
            return -1;
        }

        SDL_GPUTexture* swapchainTexture;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdbuf, window, &swapchainTexture, NULL, NULL)) {
            SDL_Log("WaitAndAcquireGPUSwapchainTexture failed: %s", SDL_GetError());
            return -1;
        }

        if (swapchainTexture != NULL)
        {
            SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
            colorTargetInfo.texture = swapchainTexture;
            colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
            colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
                cmdbuf,
                &colorTargetInfo,
                1,
                NULL
            );

            SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
            SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){ .buffer = vertex_buffer, .offset = 0 }, 1);

			SDL_PushGPUVertexUniformData(cmdbuf, 0, &add, sizeof(float) * 3);

			SDL_PushGPUFragmentUniformData(cmdbuf, 0, &sub, sizeof(float) * 3);

            SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);

    }

    return 0;
}