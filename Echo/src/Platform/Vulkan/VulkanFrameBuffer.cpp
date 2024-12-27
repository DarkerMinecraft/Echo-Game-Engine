#include "pch.h"
#include "VulkanFrameBuffer.h"
#include "Utils/VulkanInitializers.h"
#include "VulkanTexture.h"

#include "VulkanSwapchain.h"

namespace Echo 
{

	

	VulkanFrameBuffer::VulkanFrameBuffer(VulkanDevice* device, const FrameBufferDesc& frameBufferDescription)
		: m_Device(device)
	{
		CreateFrameBuffer(frameBufferDescription);
	}

	VulkanFrameBuffer::~VulkanFrameBuffer()
	{

	}

	void VulkanFrameBuffer::Start()
	{
		VkCommandBuffer cmd = m_Device->GetActiveCommandBuffer();

		std::vector<VkRenderingAttachmentInfo> colorAttachments;
		for (auto colorView : m_ColorViews)
		{
			VkRenderingAttachmentInfo colorAttachment{};
			colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachment.imageView = colorView;
			colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			if (m_ClearOnBegin)
			{
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				colorAttachment.clearValue.color = { 0.1f, 0.1f, 0.1f, 1.0f }; 
			}
			else
			{
				colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			}

			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			colorAttachments.push_back(colorAttachment);
		}

		VkRenderingAttachmentInfo depthAttachment{};
		VkRenderingAttachmentInfo* pDepthAttachment = nullptr;
		if (m_DepthView)
		{
			depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			depthAttachment.imageView = m_DepthView;
			depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			if (m_ClearOnBegin)
			{
				depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				depthAttachment.clearValue.depthStencil = { 1.0f, 0 };  
			}
			else
			{
				depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
			}
			pDepthAttachment = &depthAttachment;
		}

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea.offset = { 0, 0 };
		renderingInfo.renderArea.extent = { m_Width, m_Height };
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		renderingInfo.pColorAttachments = colorAttachments.data();
		renderingInfo.pDepthAttachment = pDepthAttachment;

		vkCmdBeginRendering(cmd, &renderingInfo);
	}

	void VulkanFrameBuffer::End()
	{
		VkCommandBuffer cmd = m_Device->GetActiveCommandBuffer();
		vkCmdEndRendering(cmd);
	}

	void VulkanFrameBuffer::CreateFrameBuffer(const FrameBufferDesc& frameBufferDescription)
	{
		m_ClearOnBegin = frameBufferDescription.ClearOnBegin;

		if (frameBufferDescription.UseSwapchain)
		{
			VulkanSwapchain* swapchain = m_Device->GetSwapchain();
			m_Width = swapchain->GetExtent().width;
			m_Height = swapchain->GetExtent().height;

			m_ColorViews.push_back(m_Device->GetDrawImage().ImageView);
		}
		else
		{
			m_Width = frameBufferDescription.Width;
			m_Height = frameBufferDescription.Height;

			for (auto& tex : frameBufferDescription.ColorAttachments)
			{
				VulkanTexture* vkTex = (VulkanTexture*)tex.get();
				if (vkTex)
				{
					m_ColorViews.push_back(vkTex->GetImageView());
				}
			}
		}

		if (frameBufferDescription.DepthAttachment)
		{
			VulkanTexture* vkDepth = (VulkanTexture*) (frameBufferDescription.DepthAttachment.get());
			if (vkDepth)
			{
				m_DepthView = vkDepth->GetImageView();
			}
		}

	}

}