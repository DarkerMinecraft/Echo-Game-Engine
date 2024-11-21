#pragma once

#include "Echo/Core/Layer.h"

#include "Echo/Scene/Scene.h"
#include "Echo/Graphics/Device.h"

namespace Echo
{
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
		Device* m_Device;

		Ref<Scene> m_ActiveScene;

		Ref<Pipeline> m_GradientPipeline;
	};
}

