#include "pch.h"
#include "VulkanDispatchCommand.h"

namespace Echo 
{

	void VulkanDispatchCommand::Execute(CommandBuffer* cmd)
	{
		EC_PROFILE_FUNCTION();
		vkCmdDispatch(((VulkanCommandBuffer*)cmd)->GetCommandBuffer(), m_X, m_Y, m_Z);
	}

}