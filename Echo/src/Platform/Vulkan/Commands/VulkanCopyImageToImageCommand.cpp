#include "pch.h"
#include "VulkanCopyImageToImageCommand.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include "Platform/Vulkan/Utils/VulkanImages.h"
#include "Platform/Vulkan/VulkanImage.h"

#include "Echo/Core/Application.h"

namespace Echo
{

	void VulkanCopyImageToImageCommand::Execute(CommandBuffer* cmd)
	{
		VulkanCommandBuffer* vulkanCmd = (VulkanCommandBuffer*)cmd;
		VkCommandBuffer commandBuffer = vulkanCmd->GetCommandBuffer();

		VulkanImage* dstImage = (VulkanImage*)m_dstImage.get();
		VulkanImage* srcImage = (VulkanImage*)m_srcImage.get();

		if (dstImage->GetCurrentLayout() != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			dstImage->TransitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		}

		if (srcImage->GetCurrentLayout() != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			srcImage->TransitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		}


		VulkanImages::CopyImageToImage(commandBuffer, srcImage->GetImage().Image, dstImage->GetImage().Image, {srcImage->GetImage().ImageExtent.width, srcImage->GetImage().ImageExtent.height}, {dstImage->GetImage().ImageExtent.width, dstImage->GetImage().ImageExtent.height});
	}

}