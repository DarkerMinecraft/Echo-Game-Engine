#include "pch.h"
#include "VulkanRenderingCommand.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include "Platform/Vulkan/VulkanImage.h"

#include "Platform/Vulkan/Utils/VulkanInitializers.h"


namespace Echo
{

	void VulkanBeginRenderingCommand::Execute(CommandBuffer* cmd)
	{
		VulkanImage* img = (VulkanImage*)m_Image.get();
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();

		VkRenderingAttachmentInfo colorAttachment = VulkanInitializers::AttachmentInfo(img->GetImage().ImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		VkExtent2D extent = { img->GetWidth(), img->GetHeight() };
		VkRenderingInfo renderingInfo = VulkanInitializers::RenderingInfo(extent, &colorAttachment, nullptr);

		vkCmdBeginRendering(commandBuffer, &renderingInfo);

		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = extent.width;
		viewport.height = extent.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = extent.width;
		scissor.extent.height = extent.height;

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VulkanEndRenderingCommand::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		vkCmdEndRendering(commandBuffer);
	}

}	