#include "pch.h"
#include "VulkanMaterial.h"

#include "Graphics/Pipeline.h"

namespace Echo 
{


	VulkanMaterial::VulkanMaterial(Device* device, Ref<Shader> shader, Ref<Texture2D> texture, PipelineSpecification& spec)
		: m_Device(static_cast<VulkanDevice*>(device)), m_Shader(static_cast<VulkanShader*>(shader.get())), m_Texture(static_cast<VulkanTexture2D*>(texture.get())), m_PipelineSpecification(spec)
	{
		m_Pipeline = new VulkanPipeline(device, this);
	}

	VulkanMaterial::VulkanMaterial(Device* device, Ref<Shader> shader, glm::vec3 color, PipelineSpecification& spec)
		: m_Device(static_cast<VulkanDevice*>(device)), m_Shader(static_cast<VulkanShader*>(shader.get())), m_Color(color), m_PipelineSpecification(spec)
	{
		m_Pipeline = new VulkanPipeline(device, this);
	}

	void VulkanMaterial::Destroy()
	{
		m_Shader->Destroy();
		m_Pipeline->Destroy();

		if (m_Texture != nullptr)
			m_Texture->Destroy();
	}

}