#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "Interface/VulkanDevice.h"
#include "Utilities/VulkanSwapChain.h"
#include "Utilities/VulkanPipeline.h"

#include "VulkanShader.h"
#include "VulkanModel.h"

#include "backends/imgui_impl_vulkan.h"

#include <array>

namespace Echo 
{

	struct SimplePushCostantData
	{
		glm::mat2 Transform{ 1.0f };
		glm::vec2 Offset;
		alignas(16) glm::vec3 Color;
	};

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass();
		~VulkanRenderPass();

		void PushGraphicsModel(Ref<GraphicsShader> shader, Ref<GraphicsModel> graphicsModel);
		
		void Start();
		void End();
		
		void Wait();

		void SetClearValues(std::array<VkClearValue, 2> clearValues) { m_ClearValues = clearValues; }
		
		void SetDefaultImGui(ImGui_ImplVulkan_InitInfo& init_info);
		VkCommandBuffer GetCurrentCommandBuffer() { return m_CommandBuffers[m_ImageIndex]; }
	private:
		void CreateCommandBuffers();
		void BeginRecordCommandBuffer();
		void RecordCommandBuffer();
		void EndRecordCommandBuffer();
		void FreeCommandBuffers();

		void RecreateSwapChain();
		void CreatePipelineLayout();
	private:
		Scope<VulkanSwapChain> m_SwapChain;
		VulkanDevice* m_Device;

		VkRenderPassBeginInfo m_RenderPassBeginInfo{};
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::map<Ref<GraphicsShader>, VulkanPipeline*> m_Pipelines;
		std::map<VulkanPipeline*, std::vector<Ref<VulkanModel>>> m_Models;

		uint32_t m_ImageIndex;

		std::array<VkClearValue, 2> m_ClearValues;
	};
}