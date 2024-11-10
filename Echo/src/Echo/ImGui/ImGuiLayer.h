#pragma once

#include "Echo/Core/Layer.h"

#include "ImGuiVulkan.h"

namespace Echo 
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		ImGuiVulkan* m_ImGuiVulkan;
	};
}