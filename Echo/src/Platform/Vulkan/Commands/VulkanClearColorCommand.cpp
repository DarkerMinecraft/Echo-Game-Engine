#include "pch.h"
#include "VulkanClearColorCommand.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include "Platform/Vulkan/Utils/VulkanInitializers.h"

#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace Echo
{

	void VulkanClearColorCommand::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		VulkanFramebuffer* fb = (VulkanFramebuffer*)m_Framebuffer.get();

		if (fb->GetCurrentLayout(m_Index) != VK_IMAGE_LAYOUT_GENERAL) 
		{
			fb->TransitionImageLayout(commandBuffer, m_Index, VK_IMAGE_LAYOUT_GENERAL);
		}

		VkClearColorValue clearValue = { m_ClearValues.x, m_ClearValues.y, m_ClearValues.z, m_ClearValues.w };
		VkImageSubresourceRange subresourceRange = VulkanInitializers::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdClearColorImage(commandBuffer, fb->GetImage(m_Index).Image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &subresourceRange);
	}
}