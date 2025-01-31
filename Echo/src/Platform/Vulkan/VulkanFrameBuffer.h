#pragma once

#include "Echo/Graphics/FrameBuffer.h"
#include "Echo/Graphics/RHIDesc.h"

#include "VulkanDevice.h"
#include "VulkanTexture.h"

namespace Echo 
{

	class VulkanFrameBuffer : public FrameBuffer
	{
	public:
		VulkanFrameBuffer(VulkanDevice* device, const FrameBufferDesc& frameBufferDescription);
		virtual ~VulkanFrameBuffer();

		virtual void Start() override;
		virtual void End() override;
	public:
		void UpdateSwapchain(uint32_t width, uint32_t height); 

		AllocatedImage GetColorAttachment(uint32_t index);
		AllocatedImage GetDepthAttachment() { return ((VulkanTexture*)m_DepthAttachment.get())->GetAllocatedImage(); }
	private:
		void CreateFrameBuffer(const FrameBufferDesc& frameBufferDescription);
	private:
		VulkanDevice* m_Device;

		std::vector<Ref<Texture>> m_ColorAttachments;
		Ref<Texture> m_DepthAttachment;

		uint32_t m_Width, m_Height;
		bool m_HasDepthAttachment;
		bool m_ClearOnBegin;
		bool m_UseSwapchain;
		bool m_UseDrawImage;
		bool m_UseSwapchainExtent;
		bool m_UseColorAttachmentSwapchain;
	};

}