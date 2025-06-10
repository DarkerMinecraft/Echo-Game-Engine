#pragma once

#include "Graphics/Primitives/CommandBuffer.h"
#include "Graphics/Commands/ICommand.h"

#include "Vulkan/Primitives/VulkanCommandBuffer.h"

namespace Echo 
{

	class VulkanDispatchCommand : public ICommand
	{
	public:
		VulkanDispatchCommand(float x, float y, float z)
			: m_X(x), m_Y(y), m_Z(z)
		{}

		virtual void Execute(CommandBuffer* cmd) override;
	private:
		float m_X, m_Y, m_Z;
	};

}