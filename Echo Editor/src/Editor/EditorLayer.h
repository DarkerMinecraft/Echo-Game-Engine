#pragma once

#include "Echo/Core/Layer.h"

#include "Echo/Scene/Scene.h"
#include "Echo/Graphics/Device.h"

#include <Echo/Graphics/PerspectiveCamera.h>
#include <Echo/Graphics/Texture.h>

namespace Echo
{

	struct PushConstantsData
	{
		glm::vec4 Data1;
		glm::vec4 Data2;
		glm::vec4 Data3;
		glm::vec4 Data4;
	};

	struct ComputeEffect
	{
		const char* Name;

		Ref<Pipeline> ComputePipeline;

		PushConstantsData Data;
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;

		virtual void OnEvent(Event& e) override;

		virtual void OnImGuiRender() override;

		virtual void Destroy() override;
	private:
		void SetBackgroundEffects();
	private:
		Device* m_Device;

		Ref<Scene> m_ActiveScene;

		Ref<Pipeline> m_ColoredPipeline;
		Ref<Pipeline> m_TexturedMeshPipeline;

		Ref<Model> m_Rectangle;

		Ref<Texture> m_MeshTexture;
		Ref<Model> m_Mesh;

		glm::vec3 m_RectanglePos, m_RectangleRot;
		float m_RectangleScale;

		std::vector<ComputeEffect> m_BackgroundEffects;
		int m_CurrentBackgroundEffect{ 0 };

		PerspectiveCamera m_Camera;

		glm::vec3 m_CameraPosition, m_CameraRotation;
	};
}

