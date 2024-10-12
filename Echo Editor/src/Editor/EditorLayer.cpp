#include "EditorLayer.h"

namespace Echo
{
	EditorLayer::EditorLayer()
	{

	}

	void EditorLayer::OnAttach()
	{
		GraphicsPipelineData& data{};
		data.FragmentShaderPath = "assets/shaders/TriangleShader.frag";
		data.VertexShaderPath = "assets/shaders/TriangleShader.vert";

		m_Pipeline = GraphicsPipeline::Create(data);
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate()
	{

	}

	void EditorLayer::OnEvent(Event& e)
	{

	}

}