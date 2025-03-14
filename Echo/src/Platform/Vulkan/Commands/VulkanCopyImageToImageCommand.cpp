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
		AllocatedImage srcImage = ((VulkanImage*)m_srcImage.get())->GetImage();
		AllocatedImage dstImage = ((VulkanImage*)m_dstImage.get())->GetImage();

		VulkanImages::CopyImageToImage(commandBuffer, srcImage.Image, dstImage.Image, { srcImage.ImageExtent.width, srcImage.ImageExtent.height }, { dstImage.ImageExtent.width, dstImage.ImageExtent.height });

	}

}