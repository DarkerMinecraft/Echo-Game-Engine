#pragma once

#include "Graphics/Primitives/CommandBuffer.h"
#include "Graphics/Commands/ICommand.h"

namespace Echo
{
	class VulkanSetLineWidthCommand : public ICommand
	{
	public:
		VulkanSetLineWidthCommand(float lineWidth)
			: m_LineWidth(lineWidth)
		{}
		virtual void Execute(CommandBuffer* cmd);
	private:
		float m_LineWidth;
	};
}