#include "pch.h"
#include "VulkanSetLineWidthCommand.h"

#include "Vulkan/VulkanCommandBuffer.h"

namespace Echo 
{

	void VulkanSetLineWidthCommand::Execute(CommandBuffer* cmd)
	{
		VulkanCommandBuffer* commandBuffer = ((VulkanCommandBuffer*)cmd);

		vkCmdSetLineWidth(commandBuffer->GetCommandBuffer(), m_LineWidth);
	}

}