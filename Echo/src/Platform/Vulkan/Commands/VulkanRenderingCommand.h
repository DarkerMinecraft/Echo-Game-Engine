#pragma once

#include "Echo/Graphics/CommandBuffer.h"
#include "Echo/Graphics/Commands/ICommand.h"

#include "Echo/Graphics/Framebuffer.h"

#include <glm/glm.hpp>

namespace Echo 
{

	class VulkanBeginRenderingCommand : public ICommand
	{
	public:
		VulkanBeginRenderingCommand(Ref<Framebuffer> framebuffer)
			: m_Framebuffer(framebuffer)
		{}
		VulkanBeginRenderingCommand() = default;

		virtual void Execute(CommandBuffer* cmd) override;
	private:
		Ref<Framebuffer> m_Framebuffer = nullptr;
		std::vector<uint32_t> m_Indexes;
	};

	class VulkanEndRenderingCommand : public ICommand
	{
	public:
		VulkanEndRenderingCommand() = default;
		virtual void Execute(CommandBuffer* cmd) override;
	};

}