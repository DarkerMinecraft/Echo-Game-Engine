#include "pch.h"
#include "VulkanClearColorCommand.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include "Platform/Vulkan/Utils/VulkanInitializers.h"

#include "Platform/Vulkan/VulkanImage.h"

namespace Echo
{

	void VulkanClearColorCommand::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		VulkanImage* img = (VulkanImage*)m_Image.get();
		if (img->GetCurrentLayout() != VK_IMAGE_LAYOUT_GENERAL) 
		{
			img->TransitionImageLayout(commandBuffer, VK_IMAGE_LAYOUT_GENERAL);
		}

		VkClearColorValue clearValue = { m_ClearValues.x, m_ClearValues.y, m_ClearValues.z, m_ClearValues.w };
		VkImageSubresourceRange subresourceRange = VulkanInitializers::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdClearColorImage(commandBuffer, img->GetImage().Image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &subresourceRange);
	}
}