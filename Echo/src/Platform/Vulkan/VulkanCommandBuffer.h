#pragma once

#include "Echo/Graphics/CommandBuffer.h"
#include "VulkanDevice.h"

#include "Echo/Graphics/Image.h"

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
		virtual void Reset() override;

		virtual void SetSourceImage(Ref<Image> srcImage) override;
		virtual bool CanExecuteCommands() override { return m_ImageIndex != -1; }

		VkCommandBuffer GetCommandBuffer() { return m_CommandBuffer; }
	private:
		void InitCommands();
	private:
		VulkanDevice* m_Device;

		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CommandBuffer;

		VkFence m_RenderFence;
		VkSemaphore m_WaitSemaphore, m_SignalSemaphore;
		uint32_t m_ImageIndex;

		AllocatedImage m_SrcImage;
	};

}