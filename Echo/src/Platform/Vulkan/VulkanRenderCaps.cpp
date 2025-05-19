#include "pch.h"
#include "VulkanRenderCaps.h"

#include "Core/Log.h"

namespace Echo 
{

	struct RenderCaps 
	{
		VkSampleCountFlagBits MaxSampleCount;
		uint32_t MaxTextureSlots;
	};

	static RenderCaps s_RenderCaps;

	void VulkanRenderCaps::Init(VulkanDevice* device)
	{
		EC_PROFILE_FUNCTION();
		VkPhysicalDevice physicalDevice = device->GetPhysicalDevice();
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		EC_CORE_INFO("Driver Information:");
		EC_CORE_INFO("     Vendor ID: {0}", physicalDeviceProperties.vendorID);
		EC_CORE_INFO("     Device ID: {0}", physicalDeviceProperties.deviceID);
		EC_CORE_INFO("     Driver Version: {0}", physicalDeviceProperties.driverVersion);
		EC_CORE_INFO("     API Version: {0}", physicalDeviceProperties.apiVersion);
		EC_CORE_INFO("     Device Name: {0}", physicalDeviceProperties.deviceName);

		EC_CORE_INFO("Device Abilities:");
		GetSampleCount(physicalDevice);
		GetMaxTextureSlots(physicalDevice);
	}

	VkSampleCountFlagBits VulkanRenderCaps::GetSampleCount()
	{
		return s_RenderCaps.MaxSampleCount;
	}

	uint32_t VulkanRenderCaps::GetMaxTextureSlots()
	{
		return s_RenderCaps.MaxTextureSlots;
	}

	void VulkanRenderCaps::GetSampleCount(VkPhysicalDevice physicalDevice)
	{
		EC_PROFILE_FUNCTION();
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

		VkSampleCountFlags counts =
			physicalDeviceProperties.limits.framebufferColorSampleCounts &
			physicalDeviceProperties.limits.framebufferDepthSampleCounts;

		if (counts & VK_SAMPLE_COUNT_64_BIT)
		{
			EC_CORE_INFO("     64x MSAA Supported!");
			s_RenderCaps.MaxSampleCount = VK_SAMPLE_COUNT_64_BIT;
			return;
		}

		if (counts & VK_SAMPLE_COUNT_32_BIT)
		{
			EC_CORE_INFO("     32x MSAA Supported!");
			s_RenderCaps.MaxSampleCount = VK_SAMPLE_COUNT_32_BIT;
			return;
		}

		if (counts & VK_SAMPLE_COUNT_16_BIT)
		{
			EC_CORE_INFO("     16x MSAA Supported!");
			s_RenderCaps.MaxSampleCount = VK_SAMPLE_COUNT_16_BIT;
			return;
		}

		if (counts & VK_SAMPLE_COUNT_8_BIT)
		{
			EC_CORE_INFO("     8x MSAA Supported!");
			s_RenderCaps.MaxSampleCount = VK_SAMPLE_COUNT_8_BIT;
			return;
		}

		if (counts & VK_SAMPLE_COUNT_4_BIT)
		{
			EC_CORE_INFO("     4x MSAA Supported!");
			s_RenderCaps.MaxSampleCount = VK_SAMPLE_COUNT_4_BIT;
			return;
		}

		if (counts & VK_SAMPLE_COUNT_2_BIT)
		{
			EC_CORE_INFO("     2x MSAA Supported!");
			s_RenderCaps.MaxSampleCount = VK_SAMPLE_COUNT_2_BIT;
			return;
		}

		EC_CORE_INFO("     MSAA Not Supported!");
		s_RenderCaps.MaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
	}

	void VulkanRenderCaps::GetMaxTextureSlots(VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		// Check various relevant limits
		uint32_t maxSlots = properties.limits.maxPerStageDescriptorSampledImages;

		// Also consider binding limits - this is often the real limiting factor
		maxSlots = std::min(maxSlots, properties.limits.maxDescriptorSetSampledImages);

		// Check combined image sampler limits too
		maxSlots = std::min(maxSlots, properties.limits.maxPerStageDescriptorSamplers);

		// Apply practical caps based on GPU vendor and type
		uint32_t practicalCap;

		// Check vendor and type to set appropriate cap
		if (properties.vendorID == 0x10DE)
		{  // NVIDIA
			practicalCap = 32;  // NVIDIA GPUs work well with 32 texture slots
		}
		else if (properties.vendorID == 0x1002)
		{  // AMD
			practicalCap = 32;  // AMD GPUs also work well with 32
		}
		else if (properties.vendorID == 0x8086)
		{  // Intel
			practicalCap = 16;  // Intel GPUs might be more limited
		}
		else
		{
			practicalCap = 16;  // Default conservative value
		}

		// Mobile GPUs might need a lower cap
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
		{
			practicalCap = std::min(practicalCap, 16u);
		}

		// Apply the practical cap
		s_RenderCaps.MaxTextureSlots = std::min(maxSlots, practicalCap);

		EC_CORE_INFO("     Max Texture Slots: {0} (Limited from driver max of {1})",
					 s_RenderCaps.MaxTextureSlots, maxSlots);
	}

}

