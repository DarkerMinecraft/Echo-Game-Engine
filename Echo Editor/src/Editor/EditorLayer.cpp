#include "EditorLayer.h"

namespace Echo
{
	EditorLayer::EditorLayer()
	{

	}

	void EditorLayer::OnAttach()
	{
		m_PipelineLibrary = CreateRef<PipelineLibrary>();
		m_Pipeline = m_PipelineLibrary->Load("Triangle", "assets/shaders/TriangleVert.spv", "assets/shaders/TriangleFrag.spv");
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