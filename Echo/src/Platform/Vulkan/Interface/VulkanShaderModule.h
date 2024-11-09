#pragma once

#include "Echo/Graphics/Interface/IShaderModule.h"
#include <vulkan/vulkan.h>

namespace Echo 
{
	class VulkanShaderModule : public IShaderModule
	{
	public:
		VulkanShaderModule(VkShaderModule module)
			: m_Module(module) {}
		virtual ~VulkanShaderModule() = default;

		operator VkShaderModule() { return m_Module; }
	private:
		VkShaderModule m_Module;
	};
}
