#include "EditorLayer.h"

#include "Echo/Graphics/Renderer.h"

namespace Echo
{
	EditorLayer::EditorLayer()
	{

	}

	void EditorLayer::OnAttach()
	{
		m_GraphicsShader = GraphicsShader::Create("assets/shaders/TriangleShader.vert", "assets/shaders/TriangleShader.frag");

		const std::vector<GraphicsModel::Vertex>& triangleVertices =
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		};
		m_Triangle = GraphicsModel::Create(triangleVertices);

		m_ActiveScene = CreateRef<Scene>();

		Renderer::Submit(m_GraphicsShader, m_Triangle);
	}

	void EditorLayer::OnDetach()
	{
		Renderer::Wait();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		Renderer::SetClearColor({ 0.3f, 0.3f, 0.3f, 0.3f });
		Renderer::BeginScene();
		
		Renderer::EndScene();
	}

	void EditorLayer::OnEvent(Event& e)
	{

	}

	void EditorLayer::Destroy()
	{
		m_Triangle->Destroy();
		m_GraphicsShader->Destroy();
	}

}