#include "pch.h"

#include "Echo/Core/Application.h"

#include "VulkanTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Echo 
{

	VulkanTexture::VulkanTexture(const std::string& texturePath)
		: m_Device((VulkanDevice*)Application::Get().GetWindow().GetDevice())
	{
		CreateTexture(texturePath);
	}

	VulkanTexture::~VulkanTexture()
	{
		m_Device->DestroyImage(m_Image);
	}

	void VulkanTexture::Bind(Pipeline* pipeline)
	{
		VkCommandBuffer cmd = m_Device->GetCurrentCommandBuffer();

		VkDescriptorSet imageSet = (VkDescriptorSet) m_Device->GetCurrentFrame().FrameDescriptors->Allocate((VkDescriptorSetLayout) pipeline->GetDescriptorLayout());
		{
			DescriptorWriter writer;
			writer.WriteImage(0, m_Image.ImageView, m_Device->GetDefaultSamplerNearest(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

			writer.UpdateSet(m_Device->GetDevice(), imageSet);
		}

		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipelineLayout)pipeline->GetPipelineLayout(), 0, 1, &imageSet, 0, nullptr);
	}

	void VulkanTexture::CreateTexture(const std::string& texturePath)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		if (!pixels) 
		{
			EC_CORE_ERROR("Failed to load texture image {0}! Using checkerboard", texturePath);
			m_Image = m_Device->GetCheckerboardImage();
			return;
		}

		m_Image = m_Device->CreateImage(pixels, VkExtent3D{ static_cast<unsigned int>(texWidth), static_cast<unsigned int>(texHeight), 1 }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	}

}