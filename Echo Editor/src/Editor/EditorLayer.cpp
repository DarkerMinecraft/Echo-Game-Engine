#include "EditorLayer.h"

#include <Echo/Graphics/Device.h>
#include <Echo/Core/Application.h>
#include <Echo/Core/Maths.h>

#include <Echo/Graphics/Pipeline.h>

#include <glm/glm.hpp>

namespace Echo
{

	struct ColoredPushConstants 
	{
		glm::mat4 transformationMatrix;
		glm::mat4 projViewMatrix;
	};


	EditorLayer::EditorLayer()
		: m_Device(Application::Get().GetWindow().GetDevice()), m_Camera({ 0, 0, 10 }, {0, 0, 0}, 60, .001f, 1000.0f)
	{
		m_CameraPosition = m_Camera.GetPosition();
		m_CameraRotation = m_Camera.GetRotation();
	}

	void EditorLayer::OnAttach()
	{
		m_ActiveScene = CreateRef<Scene>();

		m_ColoredPipeline = Pipeline::Create(GraphicsPipeline, "assets/shaders/Colored.glsl", sizeof(ColoredPushConstants));
	
		m_RectanglePos = m_RectangleRot = { 0, 0, 0 };
		m_RectangleScale = 0.6f;

		m_TexturedMeshPipeline = Pipeline::Create(GraphicsPipeline, "assets/shaders/TexturedMesh.glsl", sizeof(ColoredPushConstants), 
		{
			DescriptorType::CombinedImageSampler
		});
		m_MeshTexture = Texture::Create("assets/textures/viking_room.png");
		m_Mesh = Model::Create("assets/objs/viking_room.obj", m_MeshTexture);

		m_Rectangle = Model::Create(
			{ 0, 1, 2, 2, 1, 3 },
			{
				{ {0.5, -0.5f, 0.0}, {1, -1}, {}, {} },
				{ {0.5, 0.5f, 0.0}, {1, 1}, {}, {} },
				{ {-0.5, -0.5f, 0.0 }, {-1, -1}, {}, {} },
				{ {-0.5, 0.5f, 0.0 }, {-1, 1}, {}, {} },
			}, m_MeshTexture
		);

		SetBackgroundEffects();
	}

	void EditorLayer::OnDetach()
	{
	
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{	
		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		ComputeEffect& selected = m_BackgroundEffects[m_CurrentBackgroundEffect];
		
		selected.ComputePipeline->Bind();
		PushConstantsData data = selected.Data;

		selected.ComputePipeline->UpdatePushConstants(&data);

		m_Device->DrawBackground();

		//m_Device->AddModel(m_TexturedMeshPipeline, m_Mesh);
		m_Device->AddModel(m_TexturedMeshPipeline, m_Rectangle);
		ColoredPushConstants coloredPush{
			Maths::CreateTransformationMatrix({0, 0, 0}, {0, 0, 0}, 1),
			m_Camera.GetProjViewMatrix()
		};

		//m_Mesh->UpdatePushConstants(&coloredPush);
		m_Rectangle->UpdatePushConstants(&coloredPush);
		
		m_Device->DrawGeometry();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (e.GetEventType() == EventType::WindowResize) 
		{
			m_Camera.UpdateProjectionMatrix();
		}
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
		ImGui::Begin("Camera");

		ImGui::InputFloat3("Position", (float*)&m_CameraPosition);
		ImGui::InputFloat3("Rotation", (float*)&m_CameraRotation);

		ImGui::End();
		ImGui::Begin("Rectangle");

		ImGui::InputFloat3("Position", (float*)&m_RectanglePos);
		ImGui::InputFloat3("Rotation", (float*)&m_RectangleRot);
		ImGui::InputFloat("Scale", (float*)&m_RectangleScale);

		ImGui::End();
	}

	void EditorLayer::Destroy()
	{
		m_Device->Wait();

		m_Rectangle.reset();
		m_ColoredPipeline.reset();
		for (auto& computeEffect : m_BackgroundEffects)
		{
			computeEffect.ComputePipeline.reset();
		}
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