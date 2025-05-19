#include "pch.h"
#include "VulkanImages.h"

#include "VulkanInitializers.h"

namespace Echo 
{

	void VulkanImages::TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
	{
		VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
		imageBarrier.pNext = nullptr;

		imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
		imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

		imageBarrier.oldLayout = currentLayout;
		imageBarrier.newLayout = newLayout;

		VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		VkImageSubresourceRange subImage = VulkanInitializers::ImageSubresourceRange(aspectMask);

		imageBarrier.subresourceRange = subImage;
		imageBarrier.image = image;

		VkDependencyInfo depInfo{};
		depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		depInfo.pNext = nullptr;

		depInfo.imageMemoryBarrierCount = 1;
		depInfo.pImageMemoryBarriers = &imageBarrier;

		vkCmdPipelineBarrier2(cmd, &depInfo);
	}

	void VulkanImages::CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize)
	{
		VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2, .pNext = nullptr };

		blitRegion.srcOffsets[1].x = srcSize.width;
		blitRegion.srcOffsets[1].y = srcSize.height;
		blitRegion.srcOffsets[1].z = 1;

		blitRegion.dstOffsets[1].x = dstSize.width;
		blitRegion.dstOffsets[1].y = dstSize.height;
		blitRegion.dstOffsets[1].z = 1;

		blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.srcSubresource.baseArrayLayer = 0;
		blitRegion.srcSubresource.layerCount = 1;
		blitRegion.srcSubresource.mipLevel = 0;

		blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blitRegion.dstSubresource.baseArrayLayer = 0;
		blitRegion.dstSubresource.layerCount = 1;
		blitRegion.dstSubresource.mipLevel = 0;

		VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2, .pNext = nullptr };
		blitInfo.dstImage = destination;
		blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		blitInfo.srcImage = source;
		blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		blitInfo.filter = VK_FILTER_LINEAR;
		blitInfo.regionCount = 1;
		blitInfo.pRegions = &blitRegion;

		vkCmdBlitImage2(cmd, &blitInfo);
	}

	void VulkanImages::CopyImageToBuffer(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, const glm::vec2& size)
	{
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { static_cast<int32_t>(size.x), static_cast<int32_t>(size.y), 0 };
		region.imageExtent = { 1, 1, 1 };

		vkCmdCopyImageToBuffer(
			cmd,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			buffer,
			1,
			&region
		);
	}

	void VulkanImages::ResolveImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize)
	{
		VkImageResolve resolveRegion{};

		// Source subresource
		resolveRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		resolveRegion.srcSubresource.mipLevel = 0;
		resolveRegion.srcSubresource.baseArrayLayer = 0;
		resolveRegion.srcSubresource.layerCount = 1;
		resolveRegion.srcOffset = { 0, 0, 0 };

		// Destination subresource
		resolveRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		resolveRegion.dstSubresource.mipLevel = 0;
		resolveRegion.dstSubresource.baseArrayLayer = 0;
		resolveRegion.dstSubresource.layerCount = 1;
		resolveRegion.dstOffset = { 0, 0, 0 };

		// Extent defines the size of the region to resolve
		resolveRegion.extent = {
			std::min(srcSize.width, dstSize.width),
			std::min(srcSize.height, dstSize.height),
			1
		};

		// Perform the resolve operation
		vkCmdResolveImage(
			cmd,
			source, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			destination, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &resolveRegion
		);
	}

}