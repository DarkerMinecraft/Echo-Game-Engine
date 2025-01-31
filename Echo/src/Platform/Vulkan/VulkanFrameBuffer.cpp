#include "pch.h"
#include "VulkanFrameBuffer.h"
#include "Utils/VulkanInitializers.h"
#include "VulkanTexture.h"

#include "VulkanSwapchain.h"
#include "Utils/VulkanImages.h"

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
		for (auto colorTex : m_ColorAttachments)
		{
			VkRenderingAttachmentInfo colorAttachment{};
			colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachment.imageView = ((VulkanTexture*)colorTex.get())->GetImageView();
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

		if (m_UseSwapchain) 
		{
			VkRenderingAttachmentInfo colorAttachment{};
			colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachment.imageView = m_Device->GetActiveImage().ImageView;
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
		if (m_HasDepthAttachment)
		{
			depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			depthAttachment.imageView = ((VulkanTexture*)m_DepthAttachment.get())->GetImageView();
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

	void VulkanFrameBuffer::UpdateSwapchain(uint32_t width, uint32_t height)
	{
		if (!m_UseSwapchain && !m_UseSwapchainExtent) return;

		m_Width = width;
		m_Height = height;

		if (m_UseSwapchain)
		{
			m_ColorAttachments.clear();

			m_ColorAttachments.push_back(m_Device->GetDrawImage());
		}
	}

	AllocatedImage VulkanFrameBuffer::GetColorAttachment(uint32_t index)
	{
		if (m_UseSwapchain)
		{
			return m_Device->GetActiveImage();
		}
		else
		{
			return ((VulkanTexture*)m_ColorAttachments[index].get())->GetAllocatedImage();
		}
	}

	void VulkanFrameBuffer::CreateFrameBuffer(const FrameBufferDesc& frameBufferDescription)
	{
		m_ClearOnBegin = frameBufferDescription.ClearOnBegin;
		m_UseSwapchain = frameBufferDescription.UseSwapchainImage;
		m_UseDrawImage = frameBufferDescription.UseDrawImage;
		m_UseSwapchainExtent = frameBufferDescription.UseSwapchainExtent;
		m_UseColorAttachmentSwapchain = frameBufferDescription.UseColorAttachmentSwapchain;
		m_HasDepthAttachment = frameBufferDescription.DepthAttachment != nullptr;

		if (frameBufferDescription.UseSwapchainImage)
		{
			VulkanSwapchain* swapchain = m_Device->GetSwapchain();
			m_Width = swapchain->GetExtent().width;
			m_Height = swapchain->GetExtent().height;
		}
		else if (frameBufferDescription.UseDrawImage) 
		{
			VulkanSwapchain* swapchain = m_Device->GetSwapchain();
			m_Width = swapchain->GetExtent().width;
			m_Height = swapchain->GetExtent().height;

			m_ColorAttachments.push_back(m_Device->GetDrawImage());
		}
		else
		{
			if (!frameBufferDescription.UseSwapchainExtent)
			{
				m_Width = frameBufferDescription.Width;
				m_Height = frameBufferDescription.Height;
			}
			else 
			{
				VulkanSwapchain* swapchain = m_Device->GetSwapchain();
				m_Width = swapchain->GetExtent().width;
				m_Height = swapchain->GetExtent().height;
			}

			for (auto& colorAttachment : frameBufferDescription.ColorAttachments) 
			{
				m_ColorAttachments.push_back(colorAttachment);
			}
		}

		if (frameBufferDescription.DepthAttachment != nullptr)
		{
			m_DepthAttachment = frameBufferDescription.DepthAttachment;
		}

	}

}