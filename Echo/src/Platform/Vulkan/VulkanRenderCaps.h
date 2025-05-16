#pragma once

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanRenderCaps
	{
	public:
		static void Init(VulkanDevice* device);

		static VkSampleCountFlagBits GetSampleCount();
		static uint32_t GetMaxTextureSlots(); 
	private:
		static void GetSampleCount(VkPhysicalDevice physicalDevice);
		static void GetMaxTextureSlots(VkPhysicalDevice physicalDevice);
	};

}