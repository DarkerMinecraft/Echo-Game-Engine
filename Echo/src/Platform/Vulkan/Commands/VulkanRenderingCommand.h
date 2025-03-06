#pragma once

#include "Echo/Graphics/CommandBuffer.h"
#include "Echo/Graphics/Commands/ICommand.h"

#include "Echo/Graphics/Image.h"

namespace Echo 
{

	class VulkanBeginRenderingCommand : public ICommand
	{
	public:
		VulkanBeginRenderingCommand(Ref<Image> image)
			: m_Image(image)
		{}
		virtual void Execute(CommandBuffer* cmd) override;
	private:
		Ref<Image> m_Image;
	};

	class VulkanEndRenderingCommand : public ICommand
	{
	public:
		VulkanEndRenderingCommand() = default;
		virtual void Execute(CommandBuffer* cmd) override;
	};

}