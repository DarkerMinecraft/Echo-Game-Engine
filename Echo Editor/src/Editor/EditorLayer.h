#pragma once

#include "Echo/Core/Layer.h"

#include "Echo/Scene/Scene.h"
#include "Echo/Graphics/Resource.h"

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
		Ref<Scene> m_ActiveScene;

		Ref<Resource> m_TriangleResource;
	};
}

