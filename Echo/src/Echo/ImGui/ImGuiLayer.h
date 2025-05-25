#pragma once

#include "Core/Layer.h"

#include "AssetManager/Assets/ShaderAsset.h"

#include "Graphics/Framebuffer.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Buffer.h"
#include "Graphics/Texture.h"

#include <imgui.h>

namespace Echo 
{

	struct VertexUniformBuffer 
	{
		glm::mat4 ProjectionMatrix;
		glm::vec2 FramebufferScale;
	};

	struct ImGuiVertex
	{
		glm::vec2 Position;
		glm::vec4 Color;
		glm::vec2 TexCoords;
	};

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;

		virtual void OnEvent(Event& e) override; 

		void Begin();
		void End();

		void Destroy();

		void SetBlockEvents(bool block) { m_BlockEvents = block; }
	private:
		void DrawImGui();
		void SetDarkThemeColors();

		void ConvertVertices(const ImDrawVert* imgui_verts, ImGuiVertex* our_verts, int count);
		void CreateFontAtlas();
	private:
		bool m_BlockEvents = true;

		const uint32_t MAX_IMGUI_VERTICES = 65536;
		const uint32_t MAX_IMGUI_INDICES = 98304;

		Ref<Framebuffer> m_ImGuiFramebuffer;
		Ref<ShaderAsset> m_Shader;
		Ref<Pipeline> m_Pipeline;
		Ref<Texture2D> m_FontAtlasTexture;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<UniformBuffer> m_ProjectionBuffer;
	};
	

}