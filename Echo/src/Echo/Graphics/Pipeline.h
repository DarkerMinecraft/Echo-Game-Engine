#pragma once

#include "RHISpecification.h"

#include "Shader.h"
#include "Texture.h"
#include "Buffer.h"

namespace Echo
{
	enum class PipelineType
	{
		Graphics,
		Compute
	};

	class Pipeline
	{
	public:
		virtual ~Pipeline() = default;

		virtual void Bind(CommandBuffer* cmd) = 0;

		virtual PipelineType GetPipelineType() = 0;

		virtual void BindResource(uint32_t binding, uint32_t set, Ref<Texture2D> texture) = 0;
		virtual void BindResource(uint32_t binding, uint32_t set, Ref<Texture2D> texture, uint32_t texIndex) = 0;
		virtual void BindResource(uint32_t binding, uint32_t set, Ref<UniformBuffer> buffer) = 0;
		virtual void BindResource(uint32_t binding, uint32_t set, Ref<Framebuffer> framebuffer, uint32_t attachmentIndex) = 0;

		virtual void ReconstructPipeline(Ref<Shader> shader) = 0;

		static Ref<Pipeline> Create(Ref<Shader> shader, const PipelineSpecification& specification);
	};
}