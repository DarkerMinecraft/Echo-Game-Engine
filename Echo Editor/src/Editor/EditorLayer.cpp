#include "EditorLayer.h"

#include <Echo/Graphics/Device.h>
#include <Echo/Core/Application.h>
#include <Echo/Graphics/Resource.h>

#include <glm/glm.hpp>

namespace Echo
{
	EditorLayer::EditorLayer()
		: m_Device(Application::Get().GetWindow().GetDevice())
	{

	}

	void EditorLayer::OnAttach()
	{
		m_ActiveScene = CreateRef<Scene>();

		ResourceCreateInfo triangleCreateInfo
		{
			AssetResource::GraphicsShader,
			"assets/shaders/TriangleShader.vert",
			"assets/shaders/TriangleShader.frag"
		};


		m_TriangleResource = Resource::Create(triangleCreateInfo);

		const std::vector<Vertex> vertices = {
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		m_TriangleModel = Model::CreateModel(vertices);

		m_Device->GetCommandBuffer()->AddMesh(m_TriangleResource, m_TriangleModel);
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		m_Device->GetCommandBuffer()->Begin();
		m_Device->GetCommandBuffer()->Submit();
		m_Device->GetCommandBuffer()->End();
	}

	void EditorLayer::OnEvent(Event& e)
	{

	}

	void EditorLayer::Destroy()
	{

	}

}