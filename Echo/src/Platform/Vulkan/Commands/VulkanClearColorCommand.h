#pragma once

#include "Echo/Graphics/Commands/ICommand.h"
#include "Echo/Graphics/Image.h"

#include <glm/glm.hpp>

namespace Echo 
{

	class VulkanClearColorCommand : public ICommand
	{
	public:
		VulkanClearColorCommand(Ref<Image> image, const glm::vec4& clearValues)
			: m_ClearValues(clearValues), m_Image(image)
		{}

		virtual void Execute(CommandBuffer* cmd) override;
	private:
		const glm::vec4 m_ClearValues;
		Ref<Image> m_Image;
	};

}