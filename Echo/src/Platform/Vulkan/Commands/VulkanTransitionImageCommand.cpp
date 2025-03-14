#include "pch.h"
#include "VulkanTransitionImageCommand.h"

#include "Platform/Vulkan/Utils/VulkanImages.h"

#include "Platform/Vulkan/VulkanCommandBuffer.h"
#include <Platform/Vulkan/VulkanImage.h>

namespace Echo 
{

	void VulkanTransitionImageCommand::Execute(CommandBuffer* cmd)
	{
		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		VulkanImage* img = (VulkanImage*)m_Image.get();

		auto MapImageLayout = [](ImageLayout layout) -> VkImageLayout
		{
			switch (layout)
			{
				case ImageLayout::General:
					return VK_IMAGE_LAYOUT_GENERAL;
				case ImageLayout::ColorAttachment:
					return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				case ImageLayout::DepthAttachment:
					return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				case ImageLayout::Undefined:
					return VK_IMAGE_LAYOUT_UNDEFINED;
				case ImageLayout::TransferSrc:
					return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				case ImageLayout::TransferDst:
					return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				case ImageLayout::ShaderReadOnly:
					return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				default:
					throw std::runtime_error("Unknown Vulkan Image Layout");
			}
		};

		img->TransitionImageLayout(commandBuffer, MapImageLayout(m_NewLayout));
	}

}