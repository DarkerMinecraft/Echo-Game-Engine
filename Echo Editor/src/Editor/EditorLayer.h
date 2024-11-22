#pragma once

#include "Echo/Core/Layer.h"

#include "Echo/Scene/Scene.h"
#include "Echo/Graphics/Device.h"

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
		Ref<Model> m_Rectangle;

		std::vector<ComputeEffect> m_BackgroundEffects;
		int m_CurrentBackgroundEffect{ 0 };
	};
}

