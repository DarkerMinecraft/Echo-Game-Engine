#pragma once

#include "Echo/Core/Layer.h"

#include "Echo/Graphics/GraphicsShader.h"
#include "Echo/Graphics/GraphicsModel.h"

#include "Echo/Scene/Scene.h"

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
		Ref<GraphicsShader> m_GraphicsShader;
		Ref<GraphicsModel> m_Triangle;

		Ref<Scene> m_ActiveScene;
	};
}

