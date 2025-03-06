#pragma once

#include "Echo/Graphics/Commands/ICommand.h"
#include "Echo/Graphics/Image.h"

namespace Echo 
{

	class VulkanTransitionImageCommand : public ICommand
	{
	public:
		VulkanTransitionImageCommand(Ref<Image> image, ImageLayout oldLayout, ImageLayout newLayout)
			: m_Image(image), m_OldLayout(oldLayout), m_NewLayout(newLayout)
		{}

		virtual void Execute(CommandBuffer* cmd) override;
	private:
		Ref<Image> m_Image;
		ImageLayout m_OldLayout, m_NewLayout;
	};

}