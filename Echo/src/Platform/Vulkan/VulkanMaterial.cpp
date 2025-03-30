#include "pch.h"
#include "VulkanMaterial.h"

namespace Echo 
{


	VulkanMaterial::VulkanMaterial(Device* device, Ref<Shader> shader, Ref<Texture2D> texture)
		: m_Device(static_cast<VulkanDevice*>(device)), m_Shader(static_cast<VulkanShader*>(shader.get())), m_Texture(static_cast<VulkanTexture2D*>(texture.get()))
	{

	}

	VulkanMaterial::VulkanMaterial(Device* device, Ref<Shader> shader, glm::vec3 color)
		: m_Device(static_cast<VulkanDevice*>(device)), m_Shader(static_cast<VulkanShader*>(shader.get())), m_Color(color)
	{

	}

	void VulkanMaterial::Destroy()
	{
		m_Shader->Destroy();
		if (m_Texture != nullptr)
			m_Texture->Destroy();
	}

}