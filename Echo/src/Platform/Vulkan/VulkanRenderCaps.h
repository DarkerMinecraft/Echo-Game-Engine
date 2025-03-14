#pragma once

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanRenderCaps
	{
	public:
		static void Init(VulkanDevice* device);

		static VkSampleCountFlagBits GetSampleCount();
	private:
		static void GetSampleCount(VkPhysicalDevice physicalDevice);
	};

}