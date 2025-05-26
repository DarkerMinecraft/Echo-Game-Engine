#include "pch.h"
#include "VulkanSetScissorCommand.h"

#include "Vulkan/VulkanCommandBuffer.h"

namespace Echo 
{

	void VulkanSetScissorCommand::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();

		VkRect2D scissor{};
		scissor.offset = { (int32_t)m_X, (int32_t)m_Y };
		scissor.extent = { m_Width, m_Height };

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

}