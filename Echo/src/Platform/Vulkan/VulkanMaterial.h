#pragma once

#include "Graphics/Material.h"
#include "Graphics/Device.h"

#include "VulkanDevice.h"

#include "VulkanShader.h"
#include "VulkanTexture.h"
#include "VulkanPipeline.h"

namespace Echo 
{

	class VulkanMaterial : public Material 
	{
	public:
		VulkanMaterial(Device* device, Ref<Shader> shader, Ref<Texture2D> texture, PipelineSpecification& pipelineSpec);
		VulkanMaterial(Device* device, Ref<Shader> shader, glm::vec3 color, PipelineSpecification& pipelineSpec);

		virtual Pipeline* GetPipeline() override { return m_Pipeline; }
		virtual Texture2D* GetTexture() override { return m_Texture; }

		virtual void Destroy() override;
	private:
		virtual Shader* GetShader() override { return m_Shader; }
		virtual PipelineSpecification& GetPipelineSpecification() override { return m_PipelineSpecification; };
	private:
		VulkanDevice* m_Device;

		VulkanShader* m_Shader;
		VulkanPipeline* m_Pipeline;

		VulkanTexture2D* m_Texture = nullptr;

		PipelineSpecification& m_PipelineSpecification;

		glm::vec3 m_Color;
	};


}