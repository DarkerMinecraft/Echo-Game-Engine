#pragma once

#include "Echo/Core/Layer.h"
#include "Echo/Graphics/Image.h"

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

		virtual void OnEvent(Event& e) override; 

		void Begin();
		void End();

		void Destroy();

		void SetBlockEvents(bool block) { m_BlockEvents = block; }
	private:
		void DrawImGui();
		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
	};
	

}