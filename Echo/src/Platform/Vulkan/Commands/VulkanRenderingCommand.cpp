#include "pch.h"
#include "VulkanRenderingCommand.h"

#include "Vulkan/VulkanCommandBuffer.h"
#include "Vulkan/VulkanFramebuffer.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanSwapchain.h"

#include "Vulkan/Utils/VulkanInitializers.h"
#include "Core/Application.h"

namespace Echo
{

	void VulkanBeginRenderingCommand::Execute(CommandBuffer* cmd)
	{
		EC_PROFILE_FUNCTION();
		VulkanCommandBuffer* commandBuffer = ((VulkanCommandBuffer*)cmd);
		VulkanFramebuffer* fb = (VulkanFramebuffer*)m_Framebuffer.get();
		VulkanDevice* device = (VulkanDevice*)Application::Get().GetWindow().GetDevice();
		VkRenderingAttachmentInfo depthAttachmentInfo = {}; 
		VkRenderingAttachmentInfo* depthAttachment = nullptr;
		std::vector<VkRenderingAttachmentInfo> colorAttachments;
		VkRenderingInfo renderingInfo = {};
		VkExtent2D extent;

		if (fb == nullptr && !((VulkanCommandBuffer*)cmd)->DrawToSwapchain())
		{
			EC_CORE_ERROR("No image set for rendering!");
			return;
		}
		else if (fb)
		{
			extent = { fb->GetWidth(), fb->GetHeight() };
			for (int i = 0; i < fb->GetFramebuffersSize(); i++)
			{
				if (fb->IsDepthTexture(i))
				{
					if (fb->GetCurrentLayout(i) != VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
						fb->TransitionImageLayout(commandBuffer->GetCommandBuffer(), i, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

					depthAttachmentInfo = VulkanInitializers::AttachmentInfo(fb->GetImage(i).ImageView, nullptr, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

					if (fb->GetImage(i).ImageFormat == VK_FORMAT_D24_UNORM_S8_UINT)
					{
						
					}

					depthAttachment = &depthAttachmentInfo;
				}
				else
				{
					if (fb->GetCurrentLayout(i) != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
						fb->TransitionImageLayout(commandBuffer->GetCommandBuffer(), i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

					colorAttachments.push_back(VulkanInitializers::AttachmentInfo(fb->GetImage(i).ImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
				}
			}
			renderingInfo = VulkanInitializers::RenderingInfo(extent, colorAttachments, depthAttachment);
		}
		else
		{
			extent = device->GetSwapchain().GetExtent();
			colorAttachments.push_back(VulkanInitializers::AttachmentInfo(device->GetSwapchainImageView(commandBuffer->GetImageIndex()), nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL));
			renderingInfo = VulkanInitializers::RenderingInfo(device->GetSwapchain().GetExtent(), colorAttachments, nullptr);
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
		EC_PROFILE_FUNCTION();
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		vkCmdEndRendering(commandBuffer);
	}

}	