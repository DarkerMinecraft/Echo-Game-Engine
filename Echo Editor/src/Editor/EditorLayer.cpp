#include "EditorLayer.h"

#include <Echo/Graphics/Device.h>
#include <Echo/Core/Application.h>

#include <Echo/Graphics/Pipeline.h>

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

		m_ColoredPipeline = Pipeline::Create(GraphicsPipeline, "assets/shaders/Colored.glsl", m_Device->GetGPUDrawSize());
		m_Rectangle = Model::Create(
			{ 0, 1, 2, 2, 1, 3 },
			{
				{ {0.5, -0.5f, 0.0}, {}, {}, {0, 0, 0, 1} },
				{ {0.5, 0.5f, 0.0}, {}, {}, {0.5, 0.5, 0.5, 1} },
				{ {-0.5, -0.5f, 0.0 }, {}, {}, {1, 0, 0, 1} },
				{ {-0.5, 0.5f, 0.0 }, {}, {}, {0, 1, 0, 1} },
			}
		);
		SetBackgroundEffects();
	}

	void EditorLayer::OnDetach()
	{
		m_ColoredPipeline.reset();
		for (auto& computeEffect : m_BackgroundEffects) 
		{
			computeEffect.ComputePipeline.reset();
		}
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{	
		ComputeEffect& selected = m_BackgroundEffects[m_CurrentBackgroundEffect];
		
		selected.ComputePipeline->Bind();
		PushConstantsData data = selected.Data;

		selected.ComputePipeline->UpdatePushConstants(&data);

		m_Device->DrawBackground();

		m_Device->AddModel(m_ColoredPipeline, m_Rectangle);
		
		m_Device->DrawGeometry();
	}

	void EditorLayer::OnEvent(Event& e)
	{

	}

	void EditorLayer::OnImGuiRender()
	{
		ImGui::Begin("Background");
		ComputeEffect& selected = m_BackgroundEffects[m_CurrentBackgroundEffect];

		ImGui::Text("Selected effect: %s", selected.Name);
		ImGui::SliderInt("Effect Index", &m_CurrentBackgroundEffect, 0, m_BackgroundEffects.size() - 1);

		ImGui::InputFloat4("Data 1", (float*)&selected.Data.Data1);
		ImGui::InputFloat4("Data 2", (float*)&selected.Data.Data2);
		ImGui::InputFloat4("Data 3", (float*)&selected.Data.Data3);
		ImGui::InputFloat4("Data 4", (float*)&selected.Data.Data4);

		ImGui::End();
	}

	void EditorLayer::Destroy()
	{
	}

	void EditorLayer::SetBackgroundEffects()
	{
		PushConstantsData gradientData;
		gradientData.Data1 = { 1, 0, 0, 1 };
		gradientData.Data2 = { 0, 0, 1, 1 };

		PushConstantsData skyData;
		skyData.Data1 = { 0.1, 0.2, 0.4, 0.97 };

		m_BackgroundEffects =
		{
			{"Gradient", Pipeline::Create(PipelineType::ComputePipeline, "assets/shaders/Gradient.comp", sizeof(PushConstantsData)), gradientData},
			{"Sky", Pipeline::Create(PipelineType::ComputePipeline, "assets/shaders/Sky.comp", sizeof(PushConstantsData)), skyData}
		};
	}

}