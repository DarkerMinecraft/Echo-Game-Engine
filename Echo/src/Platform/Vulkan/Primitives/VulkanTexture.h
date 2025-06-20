#pragma once

#include "Graphics/Primitives/Texture.h"

#include "Core/UUID.h"

#include "VulkanDevice.h"
#include "Vulkan/Utils/VulkanTypes.h"

namespace Echo 
{

	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(Device* device, const std::filesystem::path& path, const Texture2DSpecification& spec);
		VulkanTexture2D(Device* device, uint32_t width, uint32_t height, void* pixels);
		VulkanTexture2D(Device* device, const AllocatedImage& allocatedImage);
		virtual ~VulkanTexture2D();

		virtual uint32_t GetWidth() override { return m_Width; }
		virtual uint32_t GetHeight() override { return m_Height; }

		virtual void Destroy() override;
		
		void* GetImGuiResourceID() override;

		VkSampler GetSampler() { return m_Texture.Sampler; }
		AllocatedImage GetTexture() { return m_Texture; }

		virtual bool operator==(const Texture& other) const override { return m_UUID == ((VulkanTexture2D&)other).m_UUID; }

	private:
		void LoadTexture(const std::filesystem::path& path, const Texture2DSpecification& spec);
		void LoadTexture(void* pixels, bool generateSampler = false);
	private:
		VulkanDevice* m_Device;
		AllocatedImage m_Texture;

		UUID m_UUID;
		int m_ImGuiID = -1;

		VkDescriptorSet m_DescriptorSet = nullptr;

		uint32_t m_Width, m_Height, m_Channels;
		bool m_IsError = false;

		bool m_IsDestroyed = false;
	};
}