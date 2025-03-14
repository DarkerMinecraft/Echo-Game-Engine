#include "pch.h"
#include "VulkanDrawCommand.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include <Platform/Vulkan/VulkanBuffer.h>

namespace Echo 
{
	void VulkanDrawCommand::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		vkCmdDraw(commandBuffer, m_VertexCount, m_InstanceCount, m_FirstVertex, m_FirstInstance);
	}

	void VulkanDrawIndexedCommand::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		vkCmdDrawIndexed(commandBuffer, m_IndexCount, m_InstanceCount, m_FirstIndex, m_VertexOffset, m_FirstInstance);
	}

	void VulkanDrawIndexedIndirect::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		AllocatedBuffer buffer = ((VulkanIndirectBuffer*)m_Buffer.get())->GetBuffer();

		vkCmdDrawIndexedIndirect(commandBuffer, buffer.Buffer, m_Offset, m_DrawCount, m_Stride);
	}

}