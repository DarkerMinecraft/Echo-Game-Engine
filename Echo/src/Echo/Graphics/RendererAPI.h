#pragma once

#include <glm/glm.hpp>
#include "Echo/Core/Base.h"

#include "GraphicsShader.h"
#include "GraphicsModel.h"

namespace Echo
{
	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, DirectX12 = 1, Vulkan = 2
		};
	public:
		virtual void BeginScene() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void EndScene() = 0;

		virtual void Wait() = 0;

		virtual void DrawIndexed(Ref<GraphicsShader> shader, Ref<GraphicsModel> graphicsModel) = 0;
		
		virtual void Destroy() = 0;

		inline static API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}
