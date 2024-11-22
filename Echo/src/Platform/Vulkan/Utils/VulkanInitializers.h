#pragma once

#include "Echo/Graphics/Synchronization.h"
#include "Echo/Graphics/CommandList.h"

#include <vulkan/vulkan.h>

namespace Echo 
{
	class VulkanInitializers 
	{
	public:
		static VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count);
		static VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);

		static VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask);
		static VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlagBits2 stageMask, VkSemaphore semaphore);
		static VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd);

		static VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo);

		static VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
		static VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);

		static VkRenderingAttachmentInfo AttachmentInfo(VkImageView view, VkClearValue* value, VkImageLayout layout);
		static VkRenderingInfo RenderingInfo(VkExtent2D extent, VkRenderingAttachmentInfo* colorAttachment, VkRenderingAttachmentInfo* depthAttachment);

		static VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits flags, VkShaderModule module);
	};
}