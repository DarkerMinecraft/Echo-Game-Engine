#pragma once

#include "Graphics/Texture.h"

#include "Core/UUID.h"

#include "VulkanDevice.h"
#include "Utils/VulkanTypes.h"

namespace Echo 
{

	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(Device* device, const std::string& path);
		VulkanTexture2D(Device* device, uint32_t width, uint32_t height, void* pixels);
		virtual ~VulkanTexture2D();

		virtual uint32_t GetWidth() override { return m_Width; }
		virtual uint32_t GetHeight() override { return m_Height; }

		virtual void* GetResourceID() override;

		virtual void Destroy() override;

		VkSampler GetSampler() { return m_Texture.Sampler; }
		AllocatedImage GetTexture() { return m_Texture; }

		virtual bool operator==(const Texture& other) const override { return m_UUID == ((VulkanTexture2D&)other).m_UUID; }
	private:
		void LoadTexture(const std::string& path);
		void LoadTexture(void* pixels);
	private:
		VulkanDevice* m_Device;
		AllocatedImage m_Texture;

		UUID m_UUID;

		VkDescriptorSet m_DescriptorSet = nullptr;

		uint32_t m_Width, m_Height, m_Channels;
		bool m_IsError = false;

		bool m_IsDestroyed = false;
	};
}