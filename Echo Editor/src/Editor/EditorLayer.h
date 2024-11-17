#pragma once

#include "Echo/Core/Layer.h"

#include "Echo/Scene/Scene.h"
#include "Echo/Graphics/Resource.h"
#include "Echo/Graphics/Device.h"
#include "Echo/Graphics/Model.h"

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

		virtual void Destroy() override;
	private:
		Device* m_Device;

		Ref<Scene> m_ActiveScene;

		Ref<Resource> m_TriangleResource;
		Ref<Model> m_TriangleModel;
	};
}

