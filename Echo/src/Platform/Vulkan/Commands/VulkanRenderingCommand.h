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
		VulkanBeginRenderingCommand(Ref<Image> image, bool depthTexture = false)
			: m_Image(image), m_IsDepthTexture(depthTexture)
		{}
		VulkanBeginRenderingCommand() = default;

		virtual void Execute(CommandBuffer* cmd) override;
	private:
		Ref<Image> m_Image = nullptr;
		bool m_IsDepthTexture;
	};

	class VulkanEndRenderingCommand : public ICommand
	{
	public:
		VulkanEndRenderingCommand() = default;
		virtual void Execute(CommandBuffer* cmd) override;
	};

}