#pragma once

#include "Echo/Graphics/RendererAPI.h"

#include "VulkanModel.h"
#include "VulkanRenderPass.h"

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
	private:
		void CreateRenderPass();
	private:
		VulkanRenderPass* m_RenderPass;
	};
}