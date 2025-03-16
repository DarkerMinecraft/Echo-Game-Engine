#pragma once

#include "Echo/Graphics/CommandBuffer.h"
#include "VulkanDevice.h"

#include "Echo/Graphics/Framebuffer.h"

namespace Echo 
{

	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(Device* device);
		virtual ~VulkanCommandBuffer();

		virtual void Start() override;
		virtual void End() override;
		virtual void Submit() override;

		virtual void SetSourceFramebuffer(Ref<Framebuffer> framebuffer) override;
		virtual void SetDrawToSwapchain(bool drawToSwapchain) override { m_DrawToSwapchain = drawToSwapchain; };

		virtual void SetShouldPresent(bool shouldPresent) override { m_ShouldPresent = shouldPresent; };

		bool DrawToSwapchain() { return m_DrawToSwapchain; }
		uint32_t GetImageIndex() { return m_ImageIndex; }

		VkCommandBuffer GetCommandBuffer() { return m_FrameData.CommandBuffer; }
	private:
		VulkanDevice* m_Device;

		uint32_t m_ImageIndex;
		VulkanFramebuffer* m_Framebuffer;

		FrameData& m_FrameData;

		bool m_ShouldPresent = false;
		bool m_DrawToSwapchain = false;
	};

}