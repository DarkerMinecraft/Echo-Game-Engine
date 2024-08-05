#pragma once

#include "Echo/Core/Layer.h"

namespace Echo
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate() override;

		virtual void OnEvent(Event& e) override;
	};
}

