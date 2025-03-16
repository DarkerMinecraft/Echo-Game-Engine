#pragma once

#include "Echo/Graphics/CommandList.h"

#include "Echo/Graphics/Camera.h"
#include "Echo/Graphics/EditorCamera.h"
#include "Echo/Graphics/Texture.h"

#include <glm/glm.hpp>

namespace Echo 
{

	struct VertexData
	{
		glm::vec3 Position{};
		glm::vec2 Size{};
		float Rotation = 0.0f;

		glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
		Ref<Texture2D> Texture = nullptr;
		float TilingFactor = 1.0f;
	};

	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;

		float DeltaTime;

		uint32_t GetTotalVertexCount() { return QuadCount * 4; }
		uint32_t GetTotalIndexCount() { return QuadCount * 6; }
	};

	class RendererQuad
	{
	public:
		static void Init(Ref<Framebuffer> framebuffer, uint32_t index);

		static void BeginScene(CommandList& cmd, const Camera& camera, const glm::mat4& transform);
		static void BeginScene(CommandList& cmd, const EditorCamera& camera);
		static void EndScene();

		static void DrawQuad(const VertexData& data);
		static void DrawQuad(const VertexData& data, const glm::mat4& transform);

		static void Flush();
		
		static Statistics GetStats();
		static void ResetStats();

		static void Destroy();
	private:
		static void FlushAndReset();
	};
}