#include "pch.h"
#include "ImGuiRenderer.h"

#include "Core/Application.h"
#include "ImGuiTextureRegistry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Echo
{
	RendererData ImGuiRenderer::s_Data;

	void ImGuiRenderer::Init()
	{
		CreateDeviceObjects();
		CreateFontTexture();
	}

	void ImGuiRenderer::Shutdown()
	{
		if (s_Data.FontTexture)
		{
			s_Data.FontTexture->Destroy();
			s_Data.FontTexture.reset();
		}

		s_Data.Shader.reset();
		s_Data.Pipeline.reset();
		s_Data.VertexBuffer.reset();
		s_Data.IndexBuffer.reset();
		s_Data.ProjectionBuffer.reset();

		ImGuiTextureRegistry::Clear();
	}

	void ImGuiRenderer::NewFrame()
	{
		if (!s_Data.Pipeline)
			CreateDeviceObjects();
		if (!s_Data.FontTexture)
			CreateFontTexture();
	}

	void ImGuiRenderer::RenderDrawData()
	{
		ImDrawData* drawData = ImGui::GetDrawData();
		if (!drawData || drawData->CmdListsCount == 0)
			return;

		// Avoid rendering when minimized
		int fbWidth = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
		int fbHeight = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
		if (fbWidth <= 0 || fbHeight <= 0)
			return;

		CommandList cmd;
		cmd.SetDrawToSwapchain(true);
		cmd.SetShouldPresent(true);

		cmd.Begin();
		cmd.BeginRendering();

		SetupRenderState(drawData, cmd);

		// Will project scissor/clipping rectangles into framebuffer space
		ImVec2 clipOff = drawData->DisplayPos;         // (0,0) unless using multi-viewports
		ImVec2 clipScale = drawData->FramebufferScale; // (1,1) unless using retina display

		// Process each draw list
		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* drawList = drawData->CmdLists[n];

			// Check if we need to resize buffers
			uint32_t vertexCount = drawList->VtxBuffer.Size;
			uint32_t indexCount = drawList->IdxBuffer.Size;

			if (vertexCount > s_Data.MaxVertices || indexCount > s_Data.MaxIndices)
			{
				// Resize buffers
				s_Data.MaxVertices = std::max(s_Data.MaxVertices, vertexCount);
				s_Data.MaxIndices = std::max(s_Data.MaxIndices, indexCount);

				s_Data.VertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(ImGuiVertex), true);
				s_Data.IndexBuffer = IndexBuffer::Create(nullptr, s_Data.MaxIndices);
			}

			// Convert and upload vertex data
			std::vector<ImGuiVertex> vertices(vertexCount);
			for (int i = 0; i < vertexCount; i++)
			{
				const ImDrawVert& imguiVert = drawList->VtxBuffer[i];
				ImGuiVertex& vertex = vertices[i];

				vertex.Position = { imguiVert.pos.x, imguiVert.pos.y };
				vertex.TexCoords = { imguiVert.uv.x, imguiVert.uv.y };

				// Convert ImU32 color to glm::vec4
				ImU32 c = imguiVert.col;
				vertex.Color.r = ((c >> 0) & 0xFF) / 255.0f;   // R
				vertex.Color.g = ((c >> 8) & 0xFF) / 255.0f;   // G
				vertex.Color.b = ((c >> 16) & 0xFF) / 255.0f;  // B
				vertex.Color.a = ((c >> 24) & 0xFF) / 255.0f;  // A
			}

			// Convert indices to uint32_t
			std::vector<uint32_t> indices(indexCount);
			for (int i = 0; i < indexCount; i++)
			{
				indices[i] = static_cast<uint32_t>(drawList->IdxBuffer[i]);
			}

			// Upload data
			s_Data.VertexBuffer->SetData(vertices.data(), vertexCount * sizeof(ImGuiVertex));
			s_Data.IndexBuffer->SetIndices(indices);

			// Bind buffers
			cmd.BindVertexBuffer(s_Data.VertexBuffer);
			cmd.BindIndicesBuffer(s_Data.IndexBuffer);

			// Process draw commands
			for (int cmdIdx = 0; cmdIdx < drawList->CmdBuffer.Size; cmdIdx++)
			{
				const ImDrawCmd* drawCmd = &drawList->CmdBuffer[cmdIdx];

				if (drawCmd->UserCallback != nullptr)
				{
					if (drawCmd->UserCallback == ImDrawCallback_ResetRenderState)
						SetupRenderState(drawData, cmd);
					else
						drawCmd->UserCallback(drawList, drawCmd);
				}
				else
				{
					// Project scissor/clipping rectangles into framebuffer space
					ImVec2 clipMin((drawCmd->ClipRect.x - clipOff.x) * clipScale.x,
								   (drawCmd->ClipRect.y - clipOff.y) * clipScale.y);
					ImVec2 clipMax((drawCmd->ClipRect.z - clipOff.x) * clipScale.x,
								   (drawCmd->ClipRect.w - clipOff.y) * clipScale.y);

					// Skip if fully clipped
					if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
						continue;

					// Set scissor rectangle
					cmd.SetScissor((int)clipMin.x,
								   (int)(fbHeight - clipMax.y),
								   (int)(clipMax.x - clipMin.x),
								   (int)(clipMax.y - clipMin.y));

					// Bind texture
					Texture2D* texture = ImGuiTextureRegistry::GetTexture(drawCmd->TextureId);
					if (texture)
					{
						s_Data.Pipeline->BindResource(1, 0, texture);
					}

					ImGuiFramebufferBinding* framebufferBinding = ImGuiTextureRegistry::GetFramebuffer(drawCmd->TextureId);
					if (framebufferBinding)
					{
						s_Data.Pipeline->BindResource(1, 0, framebufferBinding->framebuffer, framebufferBinding->attachmentIndex);
					}

					// Draw
					cmd.DrawIndexed(drawCmd->ElemCount, 1, drawCmd->IdxOffset, drawCmd->VtxOffset, 0);
				}
			}
		}

		cmd.EndRendering();
		cmd.Execute(true);
	}

	void ImGuiRenderer::CreateDeviceObjects()
	{
		// Load shader
		s_Data.Shader = AssetRegistry::LoadAsset<ShaderAsset>("Resources/shaders/imguiShader.slang");

		FramebufferSpecification framebufferSpecs{};
		framebufferSpecs.WindowExtent = true;
		framebufferSpecs.Attachments = { FramebufferTextureFormat::BGRA8 };

		s_Data.Framebuffer = Framebuffer::Create(framebufferSpecs);

		// Create pipeline
		PipelineSpecification specs{};
		specs.EnableBlending = true;
		specs.CullMode = Cull::None;
		specs.RenderTarget = s_Data.Framebuffer; 

		s_Data.Pipeline = Pipeline::Create(s_Data.Shader->GetShader(), specs);
		s_Data.Shader->SetPipeline(s_Data.Pipeline);

		// Create buffers with initial size
		s_Data.VertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(ImGuiVertex), true);
		s_Data.IndexBuffer = IndexBuffer::Create(nullptr, s_Data.MaxIndices);

		// Create projection buffer
		struct ProjectionData
		{
			glm::mat4 ProjectionMatrix;
			glm::vec2 FramebufferScale;
		} projData{};

		s_Data.ProjectionBuffer = UniformBuffer::Create(&projData, sizeof(ProjectionData));
	}

	void ImGuiRenderer::CreateFontTexture()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Get font atlas data
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		s_Data.FontTexture = Texture2D::Create(width, height, pixels);

		// Register with texture registry
		ImTextureID textureID = ImGuiTextureRegistry::RegisterTexture(s_Data.FontTexture.get());
		io.Fonts->SetTexID(textureID);

		// Clear font data to save memory
		io.Fonts->ClearTexData();
	}

	void ImGuiRenderer::SetupRenderState(ImDrawData* drawData, CommandList& cmd)
	{
		// Setup orthographic projection matrix
		float L = drawData->DisplayPos.x;
		float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
		float T = drawData->DisplayPos.y;
		float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

		struct ProjectionData
		{
			glm::mat4 ProjectionMatrix;
			glm::vec2 FramebufferScale;
		} projData{};

		projData.ProjectionMatrix = glm::ortho(L, R, B, T, -1.0f, 1.0f);
		projData.FramebufferScale = glm::vec2(drawData->FramebufferScale.x, drawData->FramebufferScale.y);

		s_Data.ProjectionBuffer->SetData(&projData, sizeof(ProjectionData));

		// Bind pipeline and projection buffer
		cmd.BindPipeline(s_Data.Pipeline);
		s_Data.Pipeline->BindResource(0, 0, s_Data.ProjectionBuffer);
	}
}