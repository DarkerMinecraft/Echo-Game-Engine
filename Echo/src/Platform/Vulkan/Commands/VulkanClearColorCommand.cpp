#include "pch.h"
#include "VulkanClearColorCommand.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include "Platform/Vulkan/Utils/VulkanInitializers.h"

namespace Echo
{

	void VulkanClearColorCommand::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();

		VkClearColorValue clearValue = { m_ClearValues.x, m_ClearValues.y, m_ClearValues.z, m_ClearValues.w };
		VkImageSubresourceRange subresourceRange = VulkanInitializers::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

		vkCmdClearColorImage(commandBuffer, (VkImage)m_Image->GetImageHandle(), VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &subresourceRange);
	}
}