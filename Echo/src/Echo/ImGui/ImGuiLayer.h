#pragma once

#include "Echo/Core/Layer.h"

#include "Echo/Graphics/FrameBuffer.h"

namespace Echo 
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		void DrawImGui();
		void Destroy();
	private:
		Ref<FrameBuffer> m_ImGuiFrameBuffer;
	};
}