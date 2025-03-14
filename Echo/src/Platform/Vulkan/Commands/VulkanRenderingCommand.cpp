#include "pch.h"
#include "VulkanRenderingCommand.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include "Platform/Vulkan/VulkanImage.h"
#include "Platform/Vulkan/VulkanDevice.h"
#include "Platform/Vulkan/VulkanSwapchain.h"

#include "Platform/Vulkan/Utils/VulkanInitializers.h"
#include "Echo/Core/Application.h"


namespace Echo
{

	void VulkanBeginRenderingCommand::Execute(CommandBuffer* cmd)
	{
		VulkanCommandBuffer* commandBuffer = ((VulkanCommandBuffer*)cmd);
		VulkanImage* img = (VulkanImage*)m_Image.get();
		VulkanDevice* device = (VulkanDevice*)Application::Get().GetWindow().GetDevice();

		VkRenderingAttachmentInfo colorAttachment;
		VkRenderingInfo renderingInfo;

		VkExtent2D extent;
		if (img == nullptr && !((VulkanCommandBuffer*)cmd)->DrawToSwapchain())
		{
			EC_CORE_ERROR("No image set for rendering!");
			return;
		}
		else if (img) 
		{
			extent = { img->GetWidth(), img->GetHeight() };
			colorAttachment = VulkanInitializers::AttachmentInfo(img->GetImage().ImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			renderingInfo = VulkanInitializers::RenderingInfo({ img->GetWidth(), img->GetHeight() }, &colorAttachment, nullptr);
		}
		else
		{
			extent = device->GetSwapchain().GetExtent();
			colorAttachment = VulkanInitializers::AttachmentInfo(device->GetSwapchainImageView(commandBuffer->GetImageIndex()), nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
			renderingInfo = VulkanInitializers::RenderingInfo(device->GetSwapchain().GetExtent(), &colorAttachment, nullptr);
		}

		vkCmdBeginRendering(commandBuffer->GetCommandBuffer(), &renderingInfo);

		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width = extent.width;
		viewport.height = extent.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		vkCmdSetViewport(commandBuffer->GetCommandBuffer(), 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = extent.width;
		scissor.extent.height = extent.height;

		vkCmdSetScissor(commandBuffer->GetCommandBuffer(), 0, 1, &scissor);
	}

	void VulkanEndRenderingCommand::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		vkCmdEndRendering(commandBuffer);
	}

}	