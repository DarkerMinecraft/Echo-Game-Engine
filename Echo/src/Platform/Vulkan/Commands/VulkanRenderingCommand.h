#pragma once

#include "Graphics/Primitives/CommandBuffer.h"
#include "Graphics/Commands/ICommand.h"

#include "Graphics/Primitives/Framebuffer.h"

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