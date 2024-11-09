#include "pch.h"
#include "VulkanRendererAPI.h"

#include "Echo/Core/Window.h"
#include "Echo/Core/Application.h"

namespace Echo
{

	void VulkanRendererAPI::BeginScene()
	{
		m_RenderPass->Start();
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { color.r, color.g, color.b, color.a };
		clearValues[1].depthStencil = { 1.0f, 0 };

		CreateRenderPass();
		m_RenderPass->SetClearValues(clearValues);
	}

	void VulkanRendererAPI::EndScene()
	{
		m_RenderPass->End();
	}

	void VulkanRendererAPI::Wait()
	{
		m_RenderPass->Wait();
	}

	void VulkanRendererAPI::DrawIndexed(Ref<GraphicsShader> shader, Ref<GraphicsModel> graphicsModel)
	{
		CreateRenderPass();
		m_RenderPass->PushGraphicsModel(shader, graphicsModel);
	}

	void VulkanRendererAPI::Destroy()
	{
		delete m_RenderPass;
	}

	void VulkanRendererAPI::CreateRenderPass()
	{
		if (m_RenderPass == nullptr) 
		{
			m_RenderPass = new VulkanRenderPass();
		}
	}

}