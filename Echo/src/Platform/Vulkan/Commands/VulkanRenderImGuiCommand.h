#pragma once

#include "Vulkan/VulkanCommandBuffer.h"

#include "Graphics/CommandBuffer.h"
#include "Graphics/Commands/ICommand.h"

#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

namespace Echo 
{

	class VulkanRenderImGuiCommand : public ICommand
	{
	public:
		VulkanRenderImGuiCommand() = default;

		void Execute(CommandBuffer* cmd) override
		{
			VulkanCommandBuffer* vulkanCmd = (VulkanCommandBuffer*)cmd;
			VkCommandBuffer commandBuffer = vulkanCmd->GetCommandBuffer();
			
			ImGui::Render();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
		}
	};

}