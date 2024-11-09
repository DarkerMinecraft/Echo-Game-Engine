#pragma once 

#include "RenderCommand.h"

namespace Echo
{
	class Renderer
	{
	public:

		static void BeginScene();
		static void SetClearColor(const glm::vec4& color);
		static void EndScene();

		static void Wait();

		static void Submit(Ref<GraphicsShader> shader, Ref<GraphicsModel> graphicsModel);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};
}