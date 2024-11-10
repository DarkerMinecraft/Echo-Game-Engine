#pragma once

#include "Echo/Graphics/RendererAPI.h"

#include "VulkanModel.h"
#include "VulkanRenderPass.h"

#include "backends/imgui_impl_vulkan.h"

namespace Echo 
{

	class VulkanRendererAPI : public RendererAPI 
	{
	public:
		VulkanRendererAPI() = default;
		~VulkanRendererAPI() = default;
		
		virtual void BeginScene() override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void EndScene();
		
		virtual void Wait() override;

		virtual void DrawIndexed(Ref<GraphicsShader> shader, Ref<GraphicsModel> graphicsModel) override;

		virtual void Destroy() override;

		void SetDefaultImGui(ImGui_ImplVulkan_InitInfo& init_info);
		VkCommandBuffer GetCurrentCommandBuffer() { return m_RenderPass->GetCurrentCommandBuffer(); }
	private:
		void CreateRenderPass();
	private:
		VulkanRenderPass* m_RenderPass;
	};
}