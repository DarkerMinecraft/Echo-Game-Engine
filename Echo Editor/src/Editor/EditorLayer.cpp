#include "EditorLayer.h"

#include <Echo/Graphics/Device.h>
#include <Echo/Core/Application.h>

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

		m_GradientPipeline = Pipeline::Create(PipelineType::ComputePipeline, "assets/shaders/Gradient.comp");
	}

	void EditorLayer::OnDetach()
	{
		m_GradientPipeline.reset();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{		
		m_Device->DrawBackground(m_GradientPipeline);
	}

	void EditorLayer::OnEvent(Event& e)
	{

	}

	void EditorLayer::OnImGuiRender()
	{

	}

	void EditorLayer::Destroy()
	{
	}

}