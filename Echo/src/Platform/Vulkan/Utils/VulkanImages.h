#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

namespace Echo 
{

	class VulkanImages 
	{
	public:
		static void TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
		static void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);
		static void CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, const glm::vec2& size);
	};

}