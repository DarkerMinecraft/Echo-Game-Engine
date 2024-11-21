#pragma once

#include <vulkan/vulkan.h>

namespace Echo 
{

	class VulkanImages 
	{
	public:
		static void TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
		static void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);
	};

}