#pragma once 

#include "RHISpecification.h"
#include "Pipeline.h"
#include "Shader.h"
#include "Texture.h"

#include "Buffer.h"

#include <glm/glm.hpp>

namespace Echo 
{

	class Material 
	{
	public:
		virtual ~Material() = default;

		virtual void Destroy() = 0; 

		virtual Pipeline* GetPipeline() = 0;
		virtual Texture2D* GetTexture() = 0;

		static Ref<Material> Create(Ref<Shader> shader,  Ref<Texture2D> texture, PipelineSpecification& pipelineSpec);
		static Ref<Material> Create(Ref<Shader> shader, const glm::vec3& color, PipelineSpecification& pipelineSpec);
	protected:
		virtual PipelineSpecification& GetPipelineSpecification() = 0;
		virtual Shader* GetShader() = 0;
	private:
		friend class VulkanPipeline;
	};

}