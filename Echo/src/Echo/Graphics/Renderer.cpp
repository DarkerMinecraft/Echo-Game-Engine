#include "pch.h"
#include "Renderer.h"

namespace Echo 
{

	void Renderer::BeginScene()
	{
		RenderCommand::BeginScene();
	}

	void Renderer::SetClearColor(const glm::vec4& color)
	{
		RenderCommand::SetClearColor(color);
	}

	void Renderer::EndScene()
	{
		RenderCommand::EndScene();
	}

	void Renderer::Wait()
	{
		RenderCommand::Wait();
	}

	void Renderer::Submit(Ref<GraphicsShader> shader, Ref<GraphicsModel> graphicsModel)
	{
		RenderCommand::DrawIndexed(shader, graphicsModel);
	}

}