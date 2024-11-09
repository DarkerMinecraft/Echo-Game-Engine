#pragma once

#include "RendererAPI.h"

namespace Echo 
{
	class RenderCommand 
	{
	public:
		inline static void BeginScene() 
		{
			m_RendererAPI->BeginScene();
		}

		inline static void EndScene() 
		{
			m_RendererAPI->EndScene();
		}

		inline static void Wait() 
		{
			m_RendererAPI->Wait();
		}

		inline static void Destroy() 
		{
			m_RendererAPI->Destroy();
		}

		inline static void SetClearColor(const glm::vec4& color) 
		{
			m_RendererAPI->SetClearColor(color);
		}

		inline static void DrawIndexed(Ref<GraphicsShader> shader, Ref<GraphicsModel> graphicsModel)
		{
			m_RendererAPI->DrawIndexed(shader, graphicsModel);
		}
	private:
		static RendererAPI* m_RendererAPI;
	};
}