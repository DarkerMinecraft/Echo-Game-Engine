#pragma once

#include "Graphics/Commands/ICommand.h"

namespace Echo 
{

	class VulkanSetScissorCommand : public ICommand
	{
	public:
		VulkanSetScissorCommand(uint32_t x, uint32_t y, uint32_t width, uint32_t height) 
			: m_X(x), m_Y(y), m_Width(width), m_Height(height) {}

		virtual void Execute(CommandBuffer* cmd) override;
	private:
		uint32_t m_X, m_Y, m_Width, m_Height;
	};

}