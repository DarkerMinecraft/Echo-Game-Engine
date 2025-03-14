#pragma once

#include "Echo/Graphics/CommandBuffer.h"
#include "Echo/Graphics/Commands/ICommand.h"

#include "Echo/Graphics/Image.h"

#include <glm/glm.hpp>

namespace Echo 
{

	class VulkanBeginRenderingCommand : public ICommand
	{
	public:
		VulkanBeginRenderingCommand(Ref<Image> image, glm::vec4 clearColor = {1.0f, 1.0f, 1.0f, 1.0f})
			: m_Image(image), m_ClearColor(clearColor)
		{}
		VulkanBeginRenderingCommand() = default;
		virtual void Execute(CommandBuffer* cmd) override;
	private:
		Ref<Image> m_Image = nullptr;
		glm::vec4 m_ClearColor{1.0f};
	};

	class VulkanEndRenderingCommand : public ICommand
	{
	public:
		VulkanEndRenderingCommand() = default;
		virtual void Execute(CommandBuffer* cmd) override;
	};

}