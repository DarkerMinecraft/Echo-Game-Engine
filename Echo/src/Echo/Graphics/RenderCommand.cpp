#include "pch.h"
#include "RenderCommand.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Echo 
{
	RendererAPI* RenderCommand::m_RendererAPI = new VulkanRendererAPI();
}