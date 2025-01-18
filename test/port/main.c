// adapted from https://github.com/TheSpydog/SDL_gpu_examples
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL3/SDL.h>

typedef struct GameVertInput {
	float p[3];
	float n[3];
	float p2[3];
	float n2[3];
	float t[2];
} GameVertInput;

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
    void * vert_shader_bc = SDL_LoadFile("game.vert.spv", &vert_shader_bc_sz);

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
    void * frag_shader_bc = SDL_LoadFile("game.frag.spv", &frag_shader_bc_sz);

    SDL_GPUShaderCreateInfo frag_shader_info = {
        .code = frag_shader_bc,
        .code_size = frag_shader_bc_sz,
        .entrypoint = "main",
        .format = frag_format,
        .stage = SDL_GPU_SHADERSTAGE_VERTEX,
        .num_samplers = 0,
        .num_uniform_buffers = 0,
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
				.pitch = sizeof(GameVertInput)
			}},
			.num_vertex_attributes = 5,
			.vertex_attributes = (SDL_GPUVertexAttribute[]){
			// 	float p[3];
			{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
				.location = 0,
				.offset = 0
			},
			// 	float n[3];
			{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
				.location = 1,
				.offset = 1 * sizeof(float) * 3
			},
			// 	float p2[3];
			{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
				.location = 2,
				.offset = 2 * sizeof(float) * 3
			},
			// 	float n2[3];
			{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
				.location = 3,
				.offset = 3 * sizeof(float) * 3
			},
			// float t[2];
			{
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
				.location = 4,
				.offset = 4 * sizeof(float) * 3
			}}
		},
		.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
		.vertex_shader = vert_shader,
		.fragment_shader = frag_shader
	};
	fprintf(stderr, "fuck!\n");

    SDL_GPUGraphicsPipeline * pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);
	if (pipeline == NULL)
	{
		SDL_Log("Failed to create pipeline!");
		return -1;
	}
	fprintf(stderr, "fuck2!\n");

	SDL_ReleaseGPUShader(device, vert_shader);
	SDL_ReleaseGPUShader(device, frag_shader);

	// Create the vertex buffer
	SDL_GPUBuffer * vertex_buffer = SDL_CreateGPUBuffer(
		device,
		&(SDL_GPUBufferCreateInfo) {
			.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
			.size = sizeof(GameVertInput) * 3
		}
	);

	// To get data into the vertex buffer, we have to use a transfer buffer
	SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(
		device,
		&(SDL_GPUTransferBufferCreateInfo) {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = sizeof(GameVertInput) * 3
		}
	);

	GameVertInput* transferData = SDL_MapGPUTransferBuffer(
		device,
		transferBuffer,
		false
	);

	transferData[0] = (GameVertInput) {
		.p  = { 0, 0, 0 },
		.n  = { 0, 0, 0 },
		.p2 = { 0, 0, 0 },
		.n2 = { 0, 0, 0 },
		.t  = { 0, 0 },
	};
	transferData[1] = (GameVertInput) {
		.p  = { 0, 0, 0 },
		.n  = { 0, 0, 0 },
		.p2 = { 0, 0, 0 },
		.n2 = { 0, 0, 0 },
		.t  = { 0, 0 },
	};
	transferData[2] = (GameVertInput) {
		.p  = { 0, 0, 0 },
		.n  = { 0, 0, 0 },
		.p2 = { 0, 0, 0 },
		.n2 = { 0, 0, 0 },
		.t  = { 0, 0 },
	};
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
			.size = sizeof(GameVertInput) * 3
		},
		false
	);

	SDL_EndGPUCopyPass(copyPass);
	SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
	SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

	fprintf(stderr, "fuck!\n");

    bool quit = 0;

    while (!quit) {

        // inputs
		SDL_Event evt;
		while (SDL_PollEvent(&evt)) {
			if (evt.type == SDL_EVENT_QUIT) {
				quit = true;
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

			float camera_pos[4] = {0, 0, 0, 0};
			SDL_PushGPUVertexUniformData(cmdbuf, 0, camera_pos, sizeof(float) * 4);
			float model_mat_v1[4] = {0, 0, 0, 0};
			SDL_PushGPUVertexUniformData(cmdbuf, 1, model_mat_v1, sizeof(float) * 4);
			float model_mat_v2[4] = {0, 0, 0, 0};
			SDL_PushGPUVertexUniformData(cmdbuf, 2, model_mat_v2, sizeof(float) * 4);
			float model_mat_v3[4] = {0, 0, 0, 0};
			SDL_PushGPUVertexUniformData(cmdbuf, 3, model_mat_v3, sizeof(float) * 4);
			float model_mat_v4[4] = {0, 0, 0, 0};
			SDL_PushGPUVertexUniformData(cmdbuf, 4, model_mat_v4, sizeof(float) * 4);
			float mouse[2] = {0, 0};
			SDL_PushGPUVertexUniformData(cmdbuf, 5, mouse, sizeof(float) * 2);
			float blend = 0;
			SDL_PushGPUVertexUniformData(cmdbuf, 6, &blend, sizeof(float) * 1);
			float glow[3] = {NAN, NAN, NAN};
			SDL_PushGPUVertexUniformData(cmdbuf, 7, glow, sizeof(float) * 3);

            SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
            SDL_BindGPUVertexBuffers(renderPass, 0, &(SDL_GPUBufferBinding){ .buffer = vertex_buffer, .offset = 0 }, 1);
            SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

            SDL_EndGPURenderPass(renderPass);
        }

        SDL_SubmitGPUCommandBuffer(cmdbuf);

    }

    return 0;
}