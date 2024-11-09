#include "pch.h"
#include "VulkanRenderPass.h"
#include "Echo/Core/Application.h"

namespace Echo 
{

	VulkanRenderPass::VulkanRenderPass()
		: m_Device((VulkanDevice*)Application::Get().GetWindow().GetDevice())
	{
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		m_SwapChain.reset();
		for (auto& [shader, pipeline] : m_Pipelines)
		{
			delete pipeline;
		}

		vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
	}

	void VulkanRenderPass::PushGraphicsModel(Ref<GraphicsShader> shader, Ref<GraphicsModel> graphicsModel)
	{
		if (!m_Pipelines.contains(shader)) 
		{
			EC_CORE_ASSERT(m_SwapChain != nullptr, "Cannot create pipeline before swap chain")
			EC_CORE_ASSERT(m_PipelineLayout != nullptr, "Cannot create pipeline before pipeline layout")

			PipelineConfigInfo pipelineConfig{};
			VulkanPipeline::DefaultPipelineConfigInfo(pipelineConfig);

			pipelineConfig.RenderPass = m_SwapChain->GetRenderPass();
			pipelineConfig.PipelineLayout = m_PipelineLayout;

			m_Pipelines[shader] = new VulkanPipeline(shader, pipelineConfig);
		}

		VulkanPipeline* pipeline = m_Pipelines[shader];
		Ref<VulkanModel> vulkanModelRef = std::dynamic_pointer_cast<VulkanModel>(graphicsModel);

		if (m_Models.contains(pipeline)) 
		{
			auto models = m_Models[pipeline];
			models.emplace_back(vulkanModelRef);
			
			m_Models[pipeline] = models;
		}
		else 
		{
			std::vector<Ref<VulkanModel>> models{};
			models.emplace_back(vulkanModelRef);

			m_Models[pipeline] = models;
		}
	}

	void VulkanRenderPass::Start()
	{
		auto result = m_SwapChain->AcquireNextImage(&m_ImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			EC_CORE_CRITICAL("Failed to acquire swap chain image")
			return;
		}

		BeginRecordCommandBuffer();
		vkCmdBeginRenderPass(m_CommandBuffers[m_ImageIndex], &m_RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		RecordCommandBuffer();
	}

	void VulkanRenderPass::End()
	{
		EndRecordCommandBuffer();
		auto result = m_SwapChain->SubmitCommandBuffers(&m_CommandBuffers[m_ImageIndex], &m_ImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Application::Get().GetWindow().WasWindowResized()) 
		{
			Application::Get().GetWindow().ResetWindowResizedFlag();
			RecreateSwapChain();
			return;
		}
		vkResetCommandBuffer(m_CommandBuffers[m_ImageIndex], 0);
	}

	void VulkanRenderPass::Wait()
	{
		vkDeviceWaitIdle(m_Device->GetDevice());
	}

	void VulkanRenderPass::RecreateSwapChain()
	{
		auto extent = Application::Get().GetWindow().GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = Application::Get().GetWindow().GetExtent();
			Application::Get().GetWindow().Wait();
		}

		vkDeviceWaitIdle(m_Device->GetDevice());
		if (m_SwapChain == nullptr)
		{
			m_SwapChain = CreateScope<VulkanSwapChain>(extent);
		}
		else
		{
			m_SwapChain = CreateScope<VulkanSwapChain>(extent, std::move(m_SwapChain));
			if (m_SwapChain->GetImageCount() != m_CommandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}

		EC_CORE_ASSERT(m_SwapChain != nullptr, "Cannot create pipeline before swap chain")
		EC_CORE_ASSERT(m_PipelineLayout != nullptr, "Cannot create pipeline before pipeline layout")
		for (auto&[shader, pipeline] : m_Pipelines)
		{
			delete pipeline;

			PipelineConfigInfo pipelineConfig{};
			VulkanPipeline::DefaultPipelineConfigInfo(pipelineConfig);

			pipelineConfig.RenderPass = m_SwapChain->GetRenderPass();
			pipelineConfig.PipelineLayout = m_PipelineLayout;

			m_Pipelines[shader] = new VulkanPipeline(shader, pipelineConfig);
		}
	}

	void VulkanRenderPass::CreatePipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushCostantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo;
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pNext = nullptr;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		pipelineLayoutInfo.flags = 0;

		if (vkCreatePipelineLayout(m_Device->GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to create pipeline layout")
		}
	}

	void VulkanRenderPass::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_SwapChain->GetImageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_Device->GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device->GetDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to allocate command buffers")
		}

	}

	void VulkanRenderPass::BeginRecordCommandBuffer() 
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_CommandBuffers[m_ImageIndex], &beginInfo) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to begin recording command buffer")
		}

		m_RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		m_RenderPassBeginInfo.renderPass = m_SwapChain->GetRenderPass();
		m_RenderPassBeginInfo.framebuffer = m_SwapChain->GetFrameBuffer(m_ImageIndex);

		m_RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		m_RenderPassBeginInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

		m_RenderPassBeginInfo.clearValueCount = static_cast<uint32_t>(m_ClearValues.size());
		m_RenderPassBeginInfo.pClearValues = m_ClearValues.data();
	}

	void VulkanRenderPass::RecordCommandBuffer()
	{
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_SwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_SwapChain->GetSwapChainExtent() };
		vkCmdSetViewport(m_CommandBuffers[m_ImageIndex], 0, 1, &viewport);
		vkCmdSetScissor(m_CommandBuffers[m_ImageIndex], 0, 1, &scissor);

		for (auto& [pipeline, models] : m_Models) 
		{
			if (pipeline != nullptr)
			{
				pipeline->Bind(m_CommandBuffers[m_ImageIndex]);

				for (auto& model : models)
				{
					model->Bind(m_CommandBuffers[m_ImageIndex]);
					model->Draw(m_CommandBuffers[m_ImageIndex]);
				}
			}
		}

		for (int i = 0; i < 4; i++)
		{
			SimplePushCostantData push{};
			push.Offset = { 0.0f, -0.4f + i * 0.25f };
			push.Color = { 0.0f, 0.0f, 0.2f + 0.2f * i };

			//vkCmdPushConstants(m_CommandBuffers[m_ImageIndex], m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushCostantData), &push);
			//for (VulkanModel* model : m_VulkanModels)
			//{
				//model->Draw(m_CommandBuffers[m_ImageIndex]);
			//}
		}
	}

	void VulkanRenderPass::EndRecordCommandBuffer()
	{
		vkCmdEndRenderPass(m_CommandBuffers[m_ImageIndex]);
		if (vkEndCommandBuffer(m_CommandBuffers[m_ImageIndex]) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to record command buffer")
		}
	}

	void VulkanRenderPass::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_Device->GetDevice(),
							 m_Device->GetCommandPool(),
							 static_cast<uint32_t>(m_CommandBuffers.size()),
							 m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

}