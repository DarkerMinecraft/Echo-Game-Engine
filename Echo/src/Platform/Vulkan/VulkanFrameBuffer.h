#pragma once

#include "Echo/Graphics/FrameBuffer.h"
#include "Echo/Graphics/RHIDesc.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanFrameBuffer : public FrameBuffer
	{
	public:
		VulkanFrameBuffer(VulkanDevice* device, const FrameBufferDesc& frameBufferDescription);
		virtual ~VulkanFrameBuffer();

		virtual void Start() override;
		virtual void End() override;
	private:
		void CreateFrameBuffer(const FrameBufferDesc& frameBufferDescription);
	private:
		VulkanDevice* m_Device;

		std::vector<VkImageView> m_ColorViews;
		VkImageView m_DepthView = VK_NULL_HANDLE;

		uint32_t m_Width, m_Height;
		bool m_ClearOnBegin;
	};

}