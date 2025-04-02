#pragma once

#include "Graphics/Material.h"
#include "Graphics/Device.h"

#include "VulkanDevice.h"

#include "VulkanShader.h"
#include "VulkanTexture.h"

namespace Echo 
{

	class VulkanMaterial : public Material 
	{
	public:
		VulkanMaterial(Device* device, Ref<Shader> shader, Ref<Texture2D> texture);
		VulkanMaterial(Device* device, Ref<Shader> shader, glm::vec3 color);

		virtual Shader* GetShader() override { return m_Shader; }
		virtual Texture2D* GetTexture() override { return m_Texture; }

		virtual void Destroy() override;
	private:
		VulkanDevice* m_Device;

		VulkanShader* m_Shader;
		VulkanTexture2D* m_Texture = nullptr;

		glm::vec3 m_Color;
	};


}