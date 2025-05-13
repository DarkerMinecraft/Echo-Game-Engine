#include "pch.h"
#include "RendererQuad.h"

#include "Graphics/Buffer.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
#include "Graphics/Shader.h"

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

	struct BatchUniformBuffer
	{
		glm::mat4 ProjViewMatrix{};
	};

	struct RendererQuadData
	{
		uint32_t MaxQuads = 10000;
		uint32_t MaxVertices = MaxQuads * 4;
		uint32_t MaxIndices = MaxQuads * 6;

		static const uint32_t MaxTextureSlots = 32;

		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;

		Ref<Material> QuadMaterial;
		Ref<Shader> QuadShader;

		Ref<UniformBuffer> QuadUniformBuffer;
		Ref<UniformBuffer> QuadHighlightBuffer;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
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

		int SelectedEntity = -1; 
		glm::vec4 HighlightColor = { 1.0f, 0.5f, 0.0f, 0.7f };

		Statistics Stats;
		CommandList* Cmd;
	};

	static RendererQuadData s_Data;

	void RendererQuad::Init(Ref<Framebuffer> framebuffer, uint32_t index)
	{
		ShaderSpecification shaderSpecs{};
		shaderSpecs.VertexShaderPath = "assets/shaders/quadVertex.slang";
		shaderSpecs.FragmentShaderPath = "assets/shaders/quadFragment.slang";

		PipelineSpecification pipelineSpec{};
		pipelineSpec.EnableBlending = false;
		pipelineSpec.EnableDepthTest = false;
		pipelineSpec.EnableDepthWrite = false;
		pipelineSpec.EnableCulling = false;
		pipelineSpec.CullMode = Cull::None;
		pipelineSpec.FillMode = Fill::Solid;
		pipelineSpec.DepthCompareOp = CompareOp::Less;
		pipelineSpec.GraphicsTopology = Topology::TriangleList;

		pipelineSpec.RenderTarget = framebuffer;

		pipelineSpec.DescriptionSetLayouts =
		{
			{0, DescriptorType::UniformBuffer, 1, ShaderStage::Vertex},
			{1, DescriptorType::SampledImage, s_Data.MaxTextureSlots, ShaderStage::Fragment }
		};

		pipelineSpec.VertexLayout =
		{
			{ ShaderDataType::Float3, "Position" },
			{ ShaderDataType::Float2, "TexCoord" },
			{ ShaderDataType::Float4, "Color" },
			{ ShaderDataType::Int, "TexIndex" },
			{ ShaderDataType::Float, "TilingFactor" },
			{ ShaderDataType::Int, "InstanceID" },
		};

		s_Data.QuadShader = Shader::Create(shaderSpecs);
		s_Data.QuadMaterial = Material::Create(s_Data.QuadShader, {1, 1, 1}, pipelineSpec);

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex), true);

		s_Data.TextureSlots[0] = Texture2D::Create(1, 1, new uint32_t(0xffffffff));

		BatchUniformBuffer batchUniformBuffer{};
		s_Data.QuadUniformBuffer = UniformBuffer::Create(&batchUniformBuffer, sizeof(BatchUniformBuffer));

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

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

	void RendererQuad::BeginScene(CommandList& cmd, const Camera& camera, const glm::mat4& transform)
	{
		s_Data.Cmd = &cmd;

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;

		cmd.BindPipeline(s_Data.QuadMaterial->GetPipeline());

		glm::mat4 projView = camera.GetProjection() * glm::inverse(transform);

		BatchUniformBuffer batchUniformBuffer
		{
			.ProjViewMatrix = projView,
		};

		s_Data.QuadUniformBuffer->SetData(&batchUniformBuffer, sizeof(BatchUniformBuffer));
		s_Data.QuadMaterial->GetPipeline()->WriteDescriptorUniformBuffer(s_Data.QuadUniformBuffer, 0);

		s_Data.Cmd->BindVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.Cmd->BindIndicesBuffer(s_Data.QuadIndexBuffer);
	}

	void RendererQuad::BeginScene(CommandList& cmd, const EditorCamera& camera)
	{
		s_Data.Cmd = &cmd;

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.TextureSlotIndex = 1;

		cmd.BindPipeline(s_Data.QuadMaterial->GetPipeline());

		BatchUniformBuffer batchUniformBuffer
		{
			.ProjViewMatrix = camera.GetProjection() * camera.GetViewMatrix(),
		};
		s_Data.QuadUniformBuffer->SetData(&batchUniformBuffer, sizeof(BatchUniformBuffer));
		s_Data.QuadMaterial->GetPipeline()->WriteDescriptorUniformBuffer(s_Data.QuadUniformBuffer, 0);

		s_Data.Cmd->BindVertexBuffer(s_Data.QuadVertexBuffer);
		s_Data.Cmd->BindIndicesBuffer(s_Data.QuadIndexBuffer);
	}

	void RendererQuad::EndScene()
	{
		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;

		if (dataSize == 0)
			return;

		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
		Flush();
	}

	void RendererQuad::DrawQuad(const VertexData& data)
	{
		
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

	void RendererQuad::DrawQuad(const VertexData& data, const glm::mat4& transform)
	{
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

	void RendererQuad::SetSelectedEntity(int entityID)
	{
		s_Data.SelectedEntity = entityID;
	}

	void RendererQuad::ClearSelection()
	{
		s_Data.SelectedEntity = -1;
	}

	uint32_t RendererQuad::GetSelectedEntity()
	{
		return s_Data.SelectedEntity;
	}

	void RendererQuad::Flush()
	{
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		{
			s_Data.QuadMaterial->GetPipeline()->WriteDescriptorCombinedTextureArray(s_Data.TextureSlots[i], i, 1);
		}

		s_Data.Cmd->DrawIndexed(s_Data.QuadIndexCount, 1, 0, 0, 0);
		s_Data.Stats.DrawCalls++;
	}

	void RendererQuad::FlushAndReset() 
	{
		EndScene();

		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
		s_Data.QuadIndexCount = 0;

		s_Data.TextureSlotIndex = 1;
	}

	Statistics RendererQuad::GetStats()
	{
		return s_Data.Stats;
	}

	void RendererQuad::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}
	
	void RendererQuad::Destroy()
	{
		s_Data.QuadVertexBuffer.reset();
		s_Data.QuadIndexBuffer.reset();
		s_Data.QuadMaterial.reset();
		s_Data.QuadUniformBuffer.reset();
		s_Data.QuadHighlightBuffer.reset();
		s_Data.QuadShader.reset();
		s_Data.TextureSlots[0]->Destroy();

		delete[] s_Data.QuadVertexBufferBase;
	}

}