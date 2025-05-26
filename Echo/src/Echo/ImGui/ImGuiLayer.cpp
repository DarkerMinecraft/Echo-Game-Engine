#include "pch.h"
#include "ImGuiLayer.h"

#include "Graphics/CommandList.h"
#include "Core/Application.h"

#include "ImGuiTextureRegistry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <backends/imgui_impl_win32.h>

#include <ImGuizmo.h>
#include <filesystem>

namespace Echo
{

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{

	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		std::filesystem::path currentPath = std::filesystem::current_path();

		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/static/OpenSans-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/static/OpenSans-Regular.ttf", 18.0f);

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();

		Application& app = Application::Get();
		HWND window = static_cast<HWND>(app.GetWindow().GetNativeWindow());

		ImGui_ImplWin32_Init(window);
		m_ImGuiFramebuffer = Framebuffer::Create({ .WindowExtent = true, .Attachments = { BGRA8 } });

		m_Shader = AssetRegistry::LoadAsset<ShaderAsset>("Resources/shaders/imguiShader.slang");

		PipelineSpecification specs{};
		specs.EnableBlending = true;  // ImGui needs alpha blending
		specs.CullMode = Cull::None;  // Don't cull ImGui geometry
		specs.RenderTarget = m_ImGuiFramebuffer;

		m_Pipeline = Pipeline::Create(m_Shader->GetShader(), specs);
		
		m_Shader->SetPipeline(m_Pipeline);

		VertexUniformBuffer uniformBuffer{};
		uniformBuffer.FramebufferScale = glm::vec2(1.0f, 1.0f);
		uniformBuffer.ProjectionMatrix = glm::mat4(1.0f);

		m_ProjectionBuffer = UniformBuffer::Create(&uniformBuffer, sizeof(VertexUniformBuffer));

		m_VertexBuffer = VertexBuffer::Create(MAX_IMGUI_VERTICES * sizeof(ImGuiVertex), true);
		m_IndexBuffer = IndexBuffer::Create(nullptr, MAX_IMGUI_INDICES);

		CreateFontAtlas();
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnUpdate(Timestep ts)
	{
		
	}

	void ImGuiLayer::OnImGuiRender()
	{
		
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (m_BlockEvents) 
		{
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		DrawImGui();
	}

	void ImGuiLayer::DrawImGui()
	{
		Application& app = Application::Get();

		ImGui::Render();
		ImDrawData* drawData = ImGui::GetDrawData();
		if (!drawData || drawData->CmdListsCount == 0) return;

		float L = drawData->DisplayPos.x;
		float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
		float T = drawData->DisplayPos.y;
		float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

		VertexUniformBuffer uniformBuffer{};
		uniformBuffer.ProjectionMatrix = glm::ortho(L, R, B, T, -1.0f, 1.0f);
		uniformBuffer.FramebufferScale = glm::vec2(
			drawData->FramebufferScale.x,
			drawData->FramebufferScale.y
		);

		m_ProjectionBuffer->SetData(&uniformBuffer, sizeof(VertexUniformBuffer));

		CommandList cmd;
		cmd.SetShouldPresent(true);
		cmd.SetDrawToSwapchain(true);
		cmd.SetSourceFramebuffer(m_ImGuiFramebuffer);

		cmd.Begin();
		cmd.BeginRendering();
		cmd.BindPipeline(m_Pipeline);
		m_Pipeline->BindResource(0, 0, m_ProjectionBuffer);
		for (int i = 0; i < drawData->CmdListsCount; i++)
		{
			const ImDrawList* drawList = drawData->CmdLists[i];

			// Convert vertices to your format
			std::vector<ImGuiVertex> vertices(drawList->VtxBuffer.Size);
			ConvertVertices(drawList->VtxBuffer.Data, vertices.data(), drawList->VtxBuffer.Size);

			// Upload vertex data as raw bytes
			m_VertexBuffer->SetData(vertices.data(), vertices.size() * sizeof(ImGuiVertex));

			// Upload indices
			std::vector<uint32_t> indices32(drawList->IdxBuffer.Size);
			for (int idx = 0; idx < drawList->IdxBuffer.Size; idx++)
			{
				indices32[idx] = static_cast<uint32_t>(drawList->IdxBuffer.Data[idx]);
			}
			m_IndexBuffer->SetIndices(indices32);

			// Bind buffers
			cmd.BindVertexBuffer(m_VertexBuffer);
			cmd.BindIndicesBuffer(m_IndexBuffer);

			// Process draw commands
			for (int cmd_i = 0; cmd_i < drawList->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* drawCmd = &drawList->CmdBuffer[cmd_i];
				if (drawCmd->UserCallback != nullptr) continue;

				// Bind texture
				Texture2D* texture = ImGuiTextureRegistry::GetTexture(drawCmd->TextureId);
				if (texture)
				{
					m_Pipeline->BindResource(1, 0, texture);
				}
				ImGuiFramebufferBinding* framebufferBinding = ImGuiTextureRegistry::GetFramebuffer(drawCmd->TextureId);
				if (framebufferBinding) 
				{
					m_Pipeline->BindResource(1, 0, framebufferBinding->framebuffer, framebufferBinding->attachmentIndex);
				}

				// Draw
				cmd.DrawIndexed(drawCmd->ElemCount, 1, drawCmd->IdxOffset, drawCmd->VtxOffset, 0);
			}
		}
		cmd.EndRendering();
		cmd.Execute(true);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	}

	void ImGuiLayer::ConvertVertices(const ImDrawVert* imgui_verts, ImGuiVertex* our_verts, int count)
	{
		for (int i = 0; i < count; i++)
		{
			// Position (2D only)
			our_verts[i].Position.x = imgui_verts[i].pos.x;
			our_verts[i].Position.y = imgui_verts[i].pos.y;

			// Convert color from ImU32 to glm::vec4
			ImU32 c = imgui_verts[i].col;
			our_verts[i].Color.r = ((c >> 0) & 0xFF) / 255.0f;   // R
			our_verts[i].Color.g = ((c >> 8) & 0xFF) / 255.0f;   // G
			our_verts[i].Color.b = ((c >> 16) & 0xFF) / 255.0f;  // B
			our_verts[i].Color.a = ((c >> 24) & 0xFF) / 255.0f;  // A

			// UV coordinates
			our_verts[i].TexCoords.x = imgui_verts[i].uv.x;
			our_verts[i].TexCoords.y = imgui_verts[i].uv.y;
		}
	}

	void ImGuiLayer::CreateFontAtlas()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Get font atlas data from ImGui
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		// Create your custom Texture2D
		m_FontAtlasTexture = Texture2D::Create(width, height, pixels);

		// Register with your texture registry system
		ImTextureID textureID = ImGuiTextureRegistry::RegisterTexture(m_FontAtlasTexture.get());

		// Tell ImGui to use your texture
		io.Fonts->SetTexID(textureID);

		// Clear the font atlas to save memory (optional)
		io.Fonts->ClearTexData();
	}

	void ImGuiLayer::Destroy()
	{
		if (m_FontAtlasTexture)
		{
			m_FontAtlasTexture->Destroy();
			m_FontAtlasTexture.reset();
		}

		ImGuiTextureRegistry::Clear();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

}