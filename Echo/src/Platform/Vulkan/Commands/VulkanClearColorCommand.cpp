#include "pch.h"
#include "VulkanClearColorCommand.h"

#include "Vulkan/Primitives/VulkanCommandBuffer.h"
#include "Vulkan/Utils/VulkanInitializers.h"

#include "Vulkan/Primitives/VulkanFramebuffer.h"

namespace Echo
{

	void VulkanClearColorCommand::Execute(CommandBuffer* cmd)
	{
		EC_PROFILE_FUNCTION();
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		VulkanFramebuffer* fb = (VulkanFramebuffer*)m_Framebuffer.get();

		if (fb->GetCurrentLayout(m_Index) != VK_IMAGE_LAYOUT_GENERAL) 
		{
			fb->TransitionImageLayout(commandBuffer, m_Index, VK_IMAGE_LAYOUT_GENERAL);
		}

		bool shouldInt = fb->GetImage(m_Index).ImageFormat == VK_FORMAT_R32_SINT;

		VkClearColorValue clearValue;
		if (shouldInt)
		{
			clearValue.int32[0] = (int32_t)m_ClearValues.r;
			clearValue.int32[1] = (int32_t)m_ClearValues.g;
			clearValue.int32[2] = (int32_t)m_ClearValues.b;
			clearValue.int32[3] = (int32_t)m_ClearValues.a;
		}
		else
		{
			clearValue.float32[0] = m_ClearValues.r;
			clearValue.float32[1] = m_ClearValues.g;
			clearValue.float32[2] = m_ClearValues.b;
			clearValue.float32[3] = m_ClearValues.a;
		}
		VkImageSubresourceRange subresourceRange = VulkanInitializers::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdClearColorImage(commandBuffer, fb->GetImage(m_Index).Image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &subresourceRange);
	}
}