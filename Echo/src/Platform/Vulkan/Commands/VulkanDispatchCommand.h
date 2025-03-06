#pragma once

#include "Echo/Graphics/CommandBuffer.h"
#include "Echo/Graphics/Commands/ICommand.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"

namespace Echo 
{

	class VulkanDispatchCommand : public ICommand
	{
	public:
		VulkanDispatchCommand(float x, float y, float z)
			: m_X(x), m_Y(y), m_Z(z)
		{}

		virtual void Execute(CommandBuffer* cmd) override { vkCmdDispatch(((VulkanCommandBuffer*)cmd)->GetCommandBuffer(), m_X, m_Y, m_Z ); };
	private:
		float m_X, m_Y, m_Z;
	};

}