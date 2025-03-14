#include "pch.h"
#include "VulkanRenderCaps.h"

#include "Echo/Core/Log.h"

namespace Echo 
{

	struct RenderCaps 
	{
		VkSampleCountFlagBits MaxSampleCount;
	};

	static RenderCaps s_RenderCaps;

	void VulkanRenderCaps::Init(VulkanDevice* device)
	{
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
	}

	void VulkanRenderCaps::GetSampleCount(VkPhysicalDevice physicalDevice)
	{
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

	VkSampleCountFlagBits VulkanRenderCaps::GetSampleCount()
	{
		return s_RenderCaps.MaxSampleCount;
	}

}

