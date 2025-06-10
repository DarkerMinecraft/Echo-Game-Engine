#include "pch.h"
#include "Renderer2D.h"

#include "Graphics/Primitives/Buffer.h"
#include "Graphics/Primitives/Pipeline.h"
#include "Graphics/Primitives/Texture.h"
#include "Graphics/Primitives/Material.h"
#include "Graphics/Primitives/Shader.h"

#include "AssetManager/Assets/ShaderAsset.h"

#include <glm/glm.hpp>

namespace Echo
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
		glm::vec4 Color;
		int TexIndex;
		float TilingFactor;

		int InstanceID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
		glm::vec4 Color;
		float OutlineThickness;
		float Fade;

		int InstanceID;
	};

	struct LineVertex 
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct CameraUniformBuffer
	{
		glm::mat4 ProjViewMatrix{};
	};

	struct RendererQuadData
	{
		uint32_t MaxQuads = 10000;
		uint32_t MaxVertices = MaxQuads * 4;
		uint32_t MaxIndices = MaxQuads * 6;

		uint32_t MaxTextureSlots;

		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;

		Ref<ShaderAsset> QuadShader;
		Ref<Pipeline> QuadPipeline;
		Ref<ShaderAsset> CircleShader;
		Ref<Pipeline> CirclePipeline;

		Ref<ShaderAsset> LineShader;
		Ref<Pipeline> LinePipeline;

		Ref<UniformBuffer> CamUniformBuffer;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		uint32_t LineCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		std::vector<Ref<Texture2D>> TextureSlots;
		uint32_t TextureSlotIndex = 1;

		const glm::vec2 QuadTexCoords[4] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		const glm::vec4 QuadVertexPositions[4] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f, -0.5f, 0.0f, 1.0f },
			{ 0.5f, 0.5f, 0.0f, 1.0f },
			{ -0.5f, 0.5f, 0.0f, 1.0f }
		};

		Statistics Stats;
		CommandList* Cmd;
	};

	static RendererQuadData s_Data;

	void Renderer2D::Init(Ref<Framebuffer> framebuffer, uint32_t index)
	{
		EC_PROFILE_FUNCTION();

		s_Data.MaxTextureSlots = Application::Get().GetWindow().GetDevice()->GetMaxTextureSlots();

		PipelineSpecification pipelineSpec{};
		pipelineSpec.CullMode = Cull::None;
		pipelineSpec.EnableBlending = true;
		pipelineSpec.RenderTarget = framebuffer;

		s_Data.QuadShader = AssetRegistry::LoadAsset<ShaderAsset>("Resources/shaders/quadShader.slang");
		s_Data.QuadPipeline = Pipeline::Create(s_Data.QuadShader->GetShader(), pipelineSpec);
		s_Data.CircleShader = AssetRegistry::LoadAsset<ShaderAsset>("Resources/shaders/circleShader.slang");
		s_Data.CirclePipeline = Pipeline::Create(s_Data.CircleShader->GetShader(), pipelineSpec);
		pipelineSpec.GraphicsTopology = Topology::LineList;
		pipelineSpec.LineWidth = 2.0f;
		s_Data.LineShader = AssetRegistry::LoadAsset<ShaderAsset>("Resources/shaders/lineShader.slang");
		s_Data.LinePipeline = Pipeline::Create(s_Data.LineShader->GetShader(), pipelineSpec);

		s_Data.QuadShader->SetPipeline(s_Data.QuadPipeline);
		s_Data.CircleShader->SetPipeline(s_Data.CirclePipeline);
		s_Data.LineShader->SetPipeline(s_Data.LinePipeline);

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex), true);
		s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex), true);
		s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex), true);

		s_Data.TextureSlots.resize(s_Data.MaxTextureSlots);
		s_Data.TextureSlots[0] = Texture2D::Create(1, 1, new uint32_t(0xffffffff));

		CameraUniformBuffer batchUniformBuffer{};
		s_Data.CamUniformBuffer = UniformBuffer::Create(&batchUniformBuffer, sizeof(CameraUniformBuffer));

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
		s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];
		s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		s_Data.QuadIndexBuffer = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		delete[] quadIndices;
	}

	void Renderer2D::BeginScene(CommandList& cmd, const Camera& camera, const glm::mat4& transform)
	{
		EC_PROFILE_FUNCTION();
		glm::mat4 projView = camera.GetProjection() * glm::inverse(transform);

		CameraUniformBuffer camUniformBuffer
		{
			.ProjViewMatrix = projView,
		};
		s_Data.CamUniformBuffer->SetData(&camUniformBuffer, sizeof(CameraUniformBuffer));

		s_Data.Cmd = &cmd;

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		cmd.BindPipeline(s_Data.QuadPipeline);

		s_Data.QuadPipeline->BindResource(0, 0, s_Data.CamUniformBuffer);

		s_Data.Cmd->BindVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.Cmd->BindIndicesBuffer(s_Data.QuadIndexBuffer);

		cmd.BindPipeline(s_Data.CirclePipeline);

		s_Data.CirclePipeline->BindResource(0, 0, s_Data.CamUniformBuffer);

		s_Data.Cmd->BindVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.Cmd->BindIndicesBuffer(s_Data.QuadIndexBuffer);
		
		cmd.BindPipeline(s_Data.LinePipeline);
		
		s_Data.LinePipeline->BindResource(0, 0, s_Data.CamUniformBuffer);

		s_Data.Cmd->BindVertexBuffer(s_Data.LineVertexBuffer);
	}

	void Renderer2D::BeginScene(CommandList& cmd, const EditorCamera& camera)
	{
		EC_PROFILE_FUNCTION();
		CameraUniformBuffer camUniformBuffer
		{
			.ProjViewMatrix = camera.GetProjection() * camera.GetViewMatrix(),
		};
		s_Data.CamUniformBuffer->SetData(&camUniformBuffer, sizeof(CameraUniformBuffer));

		s_Data.Cmd = &cmd;

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;

		s_Data.CircleIndexCount = 0;
		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

		s_Data.LineCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		cmd.BindPipeline(s_Data.QuadPipeline);

		s_Data.QuadPipeline->BindResource(0, 0, s_Data.CamUniformBuffer);

		s_Data.Cmd->BindVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.Cmd->BindIndicesBuffer(s_Data.QuadIndexBuffer);

		cmd.BindPipeline(s_Data.CirclePipeline);

		s_Data.CirclePipeline->BindResource(0, 0, s_Data.CamUniformBuffer);

		s_Data.Cmd->BindVertexBuffer(s_Data.CircleVertexBuffer);
		s_Data.Cmd->BindIndicesBuffer(s_Data.QuadIndexBuffer);

		cmd.BindPipeline(s_Data.LinePipeline);

		s_Data.LinePipeline->BindResource(0, 0, s_Data.CamUniformBuffer);

		s_Data.Cmd->BindVertexBuffer(s_Data.LineVertexBuffer);
	}

	void Renderer2D::EndScene()
	{
		EC_PROFILE_FUNCTION();
		uint32_t quadDataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		if (quadDataSize != 0)
		{
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, quadDataSize);
		}

		uint32_t circleDataSize = (uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase;
		if (circleDataSize != 0)
		{
			s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, circleDataSize);
		}

		uint32_t lineDataSize = (uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase;
		if (lineDataSize != 0) 
		{
			s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, lineDataSize);
		}
		
		if(quadDataSize != 0 || circleDataSize != 0 || lineDataSize != 0) Flush();
	}

	void Renderer2D::DrawQuad(const VertexQuadData& data)
	{
		EC_PROFILE_FUNCTION();
		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
			FlushAndReset();

		if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots)
			FlushAndReset();

		float textureIndex = 0.0f;

		if (data.Texture != nullptr)
		{
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *data.Texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = data.Texture;
				s_Data.TextureSlotIndex++;
			}

		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), data.Position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(data.Rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { data.Size.x, data.Size.y, 1.0f });

		for (int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadTexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = data.Color;
			s_Data.QuadVertexBufferPtr->TexIndex = (int)textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = data.TilingFactor;
			s_Data.QuadVertexBufferPtr->InstanceID = data.InstanceID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const VertexQuadData& data, const glm::mat4& transform)
	{
		EC_PROFILE_FUNCTION();
		if (s_Data.QuadIndexCount >= s_Data.MaxIndices)
			FlushAndReset();

		if (s_Data.TextureSlotIndex >= s_Data.MaxTextureSlots)
			FlushAndReset();

		float textureIndex = 0.0f;

		if (data.Texture != nullptr)
		{
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			{
				if (*s_Data.TextureSlots[i].get() == *data.Texture.get())
				{
					textureIndex = (float)i;
					break;
				}
			}

			if (textureIndex == 0.0f)
			{
				textureIndex = (float)s_Data.TextureSlotIndex;
				s_Data.TextureSlots[s_Data.TextureSlotIndex] = data.Texture;
				s_Data.TextureSlotIndex++;
			}

		}

		for (int i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->TexCoord = s_Data.QuadTexCoords[i];
			s_Data.QuadVertexBufferPtr->Color = data.Color;
			s_Data.QuadVertexBufferPtr->TexIndex = (int)textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = data.TilingFactor;
			s_Data.QuadVertexBufferPtr->InstanceID = data.InstanceID;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;
		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawCircle(const VertexCircleData& data)
	{
		EC_PROFILE_FUNCTION();
		if (s_Data.CircleIndexCount >= s_Data.MaxIndices)
			FlushAndReset();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), data.Position)
			* glm::scale(glm::mat4(1.0f), { data.Size.x, data.Size.y, 1.0f });

		for (int i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = data.Color;
			s_Data.CircleVertexBufferPtr->OutlineThickness = data.OutlineThickness;
			s_Data.CircleVertexBufferPtr->Fade = data.Fade;
			s_Data.CircleVertexBufferPtr->InstanceID = data.InstanceID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;
		s_Data.Stats.CircleCount++;
	}

	void Renderer2D::DrawCircle(const VertexCircleData& data, const glm::mat4& transform)
	{
		EC_PROFILE_FUNCTION();
		if (s_Data.CircleIndexCount >= s_Data.MaxIndices)
			FlushAndReset();

		for (int i = 0; i < 4; i++)
		{
			s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
			s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
			s_Data.CircleVertexBufferPtr->Color = data.Color;
			s_Data.CircleVertexBufferPtr->OutlineThickness = data.OutlineThickness;
			s_Data.CircleVertexBufferPtr->Fade = data.Fade;
			s_Data.CircleVertexBufferPtr->InstanceID = data.InstanceID;
			s_Data.CircleVertexBufferPtr++;
		}

		s_Data.CircleIndexCount += 6;
		s_Data.Stats.CircleCount++;
	}


	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color /*= { 1.0f, 1.0f, 1.0f, 1.0f, }*/)
	{
		s_Data.LineVertexBufferPtr->Position = p0;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineVertexBufferPtr->Position = p1;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineCount += 2;
	}


	void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		DrawLine(p0, p1, color);
		DrawLine(p1, p2, color);
		DrawLine(p2, p3, color);
		DrawLine(p3, p0, color);
	}


	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color /*= { 1.0f, 1.0f, 1.0f, 1.0f, }*/)
	{
		glm::vec3 lineVertices[4];

		for (size_t i = 0; i < 4; i++) 
		{
			lineVertices[i] = transform * s_Data.QuadVertexPositions[i];
		}

		DrawLine(lineVertices[0], lineVertices[1], color);
		DrawLine(lineVertices[1], lineVertices[2], color);
		DrawLine(lineVertices[2], lineVertices[3], color);
		DrawLine(lineVertices[3], lineVertices[0], color);
	}

	void Renderer2D::Flush()
	{
		EC_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount != 0)
		{
			s_Data.Cmd->BindPipeline(s_Data.QuadPipeline);
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			{
				if (s_Data.TextureSlots[i] != nullptr)
				{
					s_Data.QuadPipeline->BindResource(1, 0, s_Data.TextureSlots[i], i);
				}
			}
			s_Data.Cmd->BindVertexBuffer(s_Data.QuadVertexBuffer);
			s_Data.Cmd->DrawIndexed(s_Data.QuadIndexCount, 1, 0, 0, 0);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.CircleIndexCount != 0)
		{
			s_Data.Cmd->BindPipeline(s_Data.CirclePipeline);
			s_Data.Cmd->BindVertexBuffer(s_Data.CircleVertexBuffer);
			s_Data.Cmd->DrawIndexed(s_Data.CircleIndexCount, 1, 0, 0, 0);
			s_Data.Stats.DrawCalls++;
		}

		if (s_Data.LineCount != 0)
		{
			s_Data.Cmd->SetLineWidth(2.0f);
			s_Data.Cmd->BindPipeline(s_Data.LinePipeline);
			s_Data.Cmd->BindVertexBuffer(s_Data.LineVertexBuffer);
			s_Data.Cmd->Draw(s_Data.LineCount, 1, 0, 0);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer2D::FlushAndReset()
	{
		EC_PROFILE_FUNCTION();
		EndScene();

		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.QuadIndexCount = 0;

		s_Data.TextureSlotIndex = 1;

		s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;
		s_Data.CircleIndexCount = 0;

		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
		s_Data.LineCount = 0;
	}

	Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	void Renderer2D::Destroy()
	{
		EC_PROFILE_FUNCTION();
		s_Data.QuadVertexBuffer.reset();
		s_Data.CircleVertexBuffer.reset();
		s_Data.LineVertexBuffer.reset();
		s_Data.QuadIndexBuffer.reset();
		s_Data.CircleShader.reset();
		s_Data.QuadShader.reset();
		s_Data.LineShader.reset();
		s_Data.QuadPipeline.reset();
		s_Data.CirclePipeline.reset();
		s_Data.LinePipeline.reset();
		s_Data.CamUniformBuffer.reset();
		s_Data.TextureSlots[0]->Destroy();

		delete[] s_Data.QuadVertexBufferBase;
		delete[] s_Data.CircleVertexBufferBase;
		delete[] s_Data.LineVertexBufferBase;
	}

}