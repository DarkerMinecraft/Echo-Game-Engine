#pragma once

#include "Echo/Graphics/Texture.h"
#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanTexture : public Texture 
	{
	public:
		VulkanTexture(const std::string& texturePath);
		virtual ~VulkanTexture();

		virtual void Bind(Pipeline* pipeline) override;
	private:
		void CreateTexture(const std::string& texturePath);
	private:
		VulkanDevice* m_Device;

		AllocatedImage m_Image;
	};

}