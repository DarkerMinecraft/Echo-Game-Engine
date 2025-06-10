#pragma once

#include "Graphics/Commands/ICommand.h"
#include "Graphics/Primitives/Framebuffer.h"

#include <glm/glm.hpp>

namespace Echo 
{

	class VulkanClearColorCommand : public ICommand
	{
	public:
		VulkanClearColorCommand(Ref<Framebuffer> framebuffer, uint32_t index, const glm::vec4& clearValues)
			: m_ClearValues(clearValues), m_Framebuffer(framebuffer), m_Index(index)
		{}

		virtual void Execute(CommandBuffer* cmd) override;
	private:
		const glm::vec4 m_ClearValues;
		Ref<Framebuffer> m_Framebuffer;
		uint32_t m_Index;
	};

}