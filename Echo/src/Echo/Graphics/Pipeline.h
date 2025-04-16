#pragma once

#include "Material.h"
#include "Framebuffer.h"
#include "CommandBuffer.h"
#include "Texture.h"
#include "Buffer.h"
#include "Shader.h"

#include <vector>
#include <string>

namespace Echo 
{

	enum PipelineType { Graphics, Compute };

	class Pipeline 
	{
	public:
		virtual ~Pipeline() = default;

		virtual void Bind(CommandBuffer* cmd) = 0;

		virtual void WriteDescriptorStorageImage(Ref<Framebuffer> framebuffer, uint32_t index, uint32_t binding = 0) = 0;

		virtual void WriteDescriptorCombinedTexture(Ref<Texture> tex, uint32_t binding = 0) = 0;
		virtual void WriteDescriptorCombinedImage(Ref<Framebuffer> framebuffer, uint32_t index, uint32_t binding = 0) = 0;

		virtual void WriteDescriptorCombinedTextureArray(Ref<Texture> tex, int index, uint32_t binding = 0) = 0;
		virtual void WriteDescriptorUniformBuffer(Ref<UniformBuffer> uniformBuffer, uint32_t binding = 0) = 0;

		virtual void Destroy() = 0;

		static Ref<Pipeline> Create(Ref<Shader> computeShader, PipelineSpecification& desc);
	};

}