#pragma once

#include "Graphics/CommandList.h"

#include "Graphics/Camera.h"
#include "Graphics/EditorCamera.h"
#include "Graphics/Texture.h"

#include "AssetManager/AssetRegistry.h"

#include <glm/glm.hpp>

namespace Echo 
{

	struct VertexQuadData
	{
		glm::vec3 Position{};
		glm::vec2 Size{};
		float Rotation = 0.0f;
		int InstanceID = 0;

		glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
		Ref<Texture2D> Texture = nullptr;
		float TilingFactor = 1.0f;
	};

	struct VertexCircleData 
	{
		glm::vec3 Position{};
		glm::vec2 Size{};
		int InstanceID = 0;

		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float OutlineThickness;
		float Fade;
	};

	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;
		uint32_t CircleCount = 0;

		uint32_t GetTotalQuadVertexCount() { return QuadCount * 4; }
		uint32_t GetTotalQuadIndexCount() { return QuadCount * 6; }

		uint32_t GetTotalCircleVertexCount() { return CircleCount * 4; }
		uint32_t GetTotalCircleIndexCount() { return CircleCount * 6; }
	};

	class Renderer2D
	{
	public:
		static void Init(Ref<Framebuffer> framebuffer, uint32_t index);

		static void BeginScene(CommandList& cmd, const Camera& camera, const glm::mat4& transform);
		static void BeginScene(CommandList& cmd, const EditorCamera& camera);
		static void EndScene();

		static void DrawQuad(const VertexQuadData& data);
		static void DrawQuad(const VertexQuadData& data, const glm::mat4& transform);

		static void DrawCircle(const VertexCircleData& data);
		static void DrawCircle(const VertexCircleData& data, const glm::mat4& transform);

		static void Flush();
		
		static Statistics GetStats();
		static void ResetStats();

		static void Destroy();
	private:
		static void FlushAndReset();
	};
}