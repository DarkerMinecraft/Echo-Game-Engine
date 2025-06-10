#pragma once

#include "AssetManager/Assets/ShaderAsset.h"

#include "Graphics/Primitives/Buffer.h"
#include "Graphics/Primitives/Texture.h"
#include "Graphics/Primitives/Pipeline.h"
#include "Graphics/Primitives/Framebuffer.h"

#include "Graphics/CommandList.h"

#include <imgui.h>

namespace Echo
{

	struct RendererData
	{
		Ref<ShaderAsset> Shader;
		Ref<Pipeline> Pipeline;
		Ref<Texture2D> FontTexture;
		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer> IndexBuffer;
		Ref<UniformBuffer> ProjectionBuffer;
		Ref<Framebuffer> Framebuffer;

		uint32_t MaxVertices = 65536;
		uint32_t MaxIndices = 98304;
	};

	struct ImGuiVertex
	{
		glm::vec2 Position;
		glm::vec4 Color;
		glm::vec2 TexCoords;
	};


	class ImGuiRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void NewFrame();
		static void RenderDrawData();
	private:
		static void CreateDeviceObjects();
		static void CreateFontTexture();
		static void SetupRenderState(ImDrawData* drawData, CommandList& cmd);
	private:
		static RendererData s_Data;
	};

}