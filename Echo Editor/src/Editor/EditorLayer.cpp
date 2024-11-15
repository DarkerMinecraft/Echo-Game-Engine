#include "EditorLayer.h"

namespace Echo
{
	EditorLayer::EditorLayer()
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
	}

	void EditorLayer::OnDetach()
	{
		
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		
	}

	void EditorLayer::OnEvent(Event& e)
	{

	}

	void EditorLayer::Destroy()
	{

	}

}