#include "pch.h"
#include "VulkanTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <backends/imgui_impl_vulkan.h>
#include "ImGui/ImGuiTextureRegistry.h"

namespace Echo
{

	static VkFilter TextureFilterToVkFilter(TextureFilter filter) 
	{
		switch(filter) 
		{
			case TextureFilter::Linear: return VK_FILTER_LINEAR;
			case TextureFilter::Nearest: return VK_FILTER_NEAREST;
		}
		return VK_FILTER_LINEAR; // Default case
	}

	VulkanTexture2D::VulkanTexture2D(Device* device, const std::filesystem::path& path, const Texture2DSpecification& spec)
		: m_Device((VulkanDevice*)device)
	{
		LoadTexture(path, spec);
	}

	VulkanTexture2D::VulkanTexture2D(Device* device, uint32_t width, uint32_t height, void* pixels)
		: m_Device((VulkanDevice*)device), m_Width(width), m_Height(height)
	{
		LoadTexture(pixels, true);
	}

	VulkanTexture2D::VulkanTexture2D(Device* device, const AllocatedImage& allocatedImage)
		: m_Device((VulkanDevice*)device), m_Texture(allocatedImage)
	{
		m_Width = allocatedImage.ImageExtent.width;
		m_Height = allocatedImage.ImageExtent.height;
		m_Channels = 4; // Assume RGBA
		m_UUID = UUID(); // Generate new UUID
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		Destroy();
	}

	void* VulkanTexture2D::GetImGuiResourceID()
	{
		if (m_DescriptorSet == nullptr)
		{
			m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_Texture.Sampler, m_Texture.ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			m_Device->AddImGuiTexture(this);
		}

		return m_DescriptorSet;
	}

	void VulkanTexture2D::Destroy()
	{
		EC_PROFILE_FUNCTION();
		if (m_IsDestroyed)
			return;

		if (m_DescriptorSet)
		{
			ImGui_ImplVulkan_RemoveTexture(m_DescriptorSet);
			m_DescriptorSet = nullptr;
		}

		vkDestroySampler(m_Device->GetDevice(), m_Texture.Sampler, nullptr);
		m_Device->DestroyImage(m_Texture);

		m_IsDestroyed = true;
	}

	void VulkanTexture2D::LoadTexture(const std::filesystem::path& path, const Texture2DSpecification& spec)
	{
		EC_PROFILE_FUNCTION();
		int width, height, channels;
		stbi_uc* data = stbi_load(path.string().c_str(), &width, &height, &channels, 4);
		if (!data)
		{
			EC_CORE_ERROR("Failed to load texture: {0}", path.string());
			EC_CORE_ERROR("Loading error texture instead!");

			m_Width = 16;
			m_Height = 16;
			m_Channels = 4;
			uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 0));

			uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
			std::array<uint32_t, 16 * 16 > pixels;
			for (int x = 0; x < 16; x++)
			{
				for (int y = 0; y < 16; y++)
				{
					pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
				}
			}
			LoadTexture(pixels.data());
			return;
		}
		else
		{
			m_Width = width;
			m_Height = height;
			m_Channels = channels;

			m_Texture = m_Device->CreateImageTex(data, { m_Width, m_Height, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
		}
		stbi_image_free(data);

		VkSamplerCreateInfo sampl = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };

		sampl.magFilter = TextureFilterToVkFilter(spec.MagFilter);
		sampl.minFilter = TextureFilterToVkFilter(spec.MinFilter);

		vkCreateSampler(m_Device->GetDevice(), &sampl, nullptr, &m_Texture.Sampler);
	}

	void VulkanTexture2D::LoadTexture(void* pixels, bool generateSampler)
	{
		EC_PROFILE_FUNCTION();
		m_Texture = m_Device->CreateImageTex(pixels, { m_Width, m_Height, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);
		
		if (generateSampler)
		{
			VkSamplerCreateInfo sampl = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };

			sampl.magFilter = VK_FILTER_NEAREST;
			sampl.minFilter = VK_FILTER_NEAREST;

			vkCreateSampler(m_Device->GetDevice(), &sampl, nullptr, &m_Texture.Sampler);
		}
	}

}