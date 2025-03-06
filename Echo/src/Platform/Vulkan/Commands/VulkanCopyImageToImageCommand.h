#pragma once

#include "Echo/Graphics/CommandBuffer.h"
#include "Echo/Graphics/Commands/ICommand.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include "Platform/Vulkan/Utils/VulkanImages.h"

namespace Echo 
{

	class VulkanCopyImageToImageCommand : public ICommand
	{
	public:
		VulkanCopyImageToImageCommand(Ref<Image> srcImage, Ref<Image> dstImage)
			: m_srcImage(srcImage), m_dstImage(dstImage)
		{}
		virtual void Execute(CommandBuffer* cmd) override;
	private:
		Ref<Image> m_srcImage;
		Ref<Image> m_dstImage;
	};

}