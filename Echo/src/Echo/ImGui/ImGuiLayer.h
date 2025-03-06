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

		void Begin();
		void End();

		void Destroy();
	private:
		void DrawImGui();
	private:
		Ref<Image> m_DrawImage;
		Ref<Image> m_ImGuiImage;
	};
	

}