#include "EditorLayer.h"

#include <Echo/Graphics/CommandList.h>
#include <Echo/Graphics/Commands/CommandFactory.h>

#include <array>
#include <imgui.h>

namespace Echo
{

	EditorLayer::EditorLayer()
	{
		
	}

	void EditorLayer::OnAttach()
	{
		m_DrawImage = Image::Create({ .DrawImage = true });

		PipelineDesc computePipelineDesc{};

		computePipelineDesc.DescriptionSetLayouts =
		{
			{0, DescriptorType::StorageImage, 1, ShaderStage::Compute}
		};
		computePipelineDesc.MaxSets = 1;

		m_ComputePipeline = Pipeline::Create("assets/shaders/gradient.hlsl", computePipelineDesc);

		PipelineDesc trianglePipelineDesc{};

		trianglePipelineDesc.EnableBlending = false;
		trianglePipelineDesc.EnableDepthTest = true;
		trianglePipelineDesc.EnableDepthWrite = true;
		trianglePipelineDesc.EnableCulling = false;
		trianglePipelineDesc.CullMode = Cull::None;
		trianglePipelineDesc.FillMode = Fill::Solid;
		trianglePipelineDesc.DepthCompareOp = CompareOp::Less;
		trianglePipelineDesc.GraphicsTopology = Topology::TriangleList;
		trianglePipelineDesc.RenderTarget = m_DrawImage;

		const uint32_t totalStride = 5 * sizeof(float);
		trianglePipelineDesc.VertexAttributes =
		{
			{ "Position", 0, 0, totalStride, VertexFormat::Float2 },
			{ "Color", 1, 0, totalStride, VertexFormat::Float3, sizeof(float) * 2 }
		};

		m_TrianglePipeline = Pipeline::Create(Material::Create("assets/shaders/triangleVertex.hlsl", "assets/shaders/triangleFragment.hlsl"), trianglePipelineDesc);

		m_TriangleVertexBuffer = VertexBuffer::Create(
			{
				1.0f, 1.0f,
				-1.0f, 1.0f,
				0.0f, -1.0f
			},
			{
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f
			}
		);
	}

	void EditorLayer::OnDetach()
	{
	
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{	
		CommandList cmdList;

		cmdList.Begin();
		cmdList.RecordCommand(CommandFactory::TransitionImageCommand(m_DrawImage, ImageLayout::Undefined, ImageLayout::General));
		cmdList.RecordCommand(CommandFactory::ClearColorCommand(m_DrawImage, { 0.8f, 0.2f, 0.7f, 1.0f }));
		//cmdList.RecordCommand(CommandFactory::BindPipelineCommand(m_ComputePipeline));
		//cmdList.RecordCommand(CommandFactory::DispatchCommand(std::ceil(m_DrawImage->GetWidth() / 16.0), std::ceil(m_DrawImage->GetHeight() / 16.0), 1));
		//m_ComputePipeline->WriteDesciptorStorageImage(m_DrawImage, 0);
		cmdList.RecordCommand(CommandFactory::TransitionImageCommand(m_DrawImage, ImageLayout::General, ImageLayout::ColorAttachment));
		cmdList.RecordCommand(CommandFactory::BeginRenderingCommand(m_DrawImage));
		cmdList.RecordCommand(CommandFactory::BindPipelineCommand(m_TrianglePipeline));
		cmdList.RecordCommand(CommandFactory::BindVertexBufferCommand(m_TriangleVertexBuffer));
		cmdList.RecordCommand(CommandFactory::DrawCommand(3, 1, 0, 0));
		cmdList.RecordCommand(CommandFactory::EndRenderingCommand());
		cmdList.RecordCommand(CommandFactory::TransitionImageCommand(m_DrawImage, ImageLayout::ColorAttachment, ImageLayout::General));
		cmdList.SetSrcImage(m_DrawImage);
		cmdList.Execute();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		
	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui::Begin("Editor");
		ImGui::Text("Hello, World!");
		ImGui::End();
	}

	void EditorLayer::Destroy()
	{
		
	}
}