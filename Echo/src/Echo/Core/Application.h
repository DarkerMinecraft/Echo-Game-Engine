#pragma once

#include "LayerStack.h"
#include "Window.h"
#include "Echo/Events/Event.h"
#include "Echo/Events/WindowEvents.h"

#include "Echo/ImGui/ImGuiLayer.h"

namespace Echo
{

	class Application
	{
	public:
		Application(unsigned int width = 1280, unsigned int height = 720, const char* title = "Echo Engine Game");
		virtual ~Application() = default;

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *m_Window; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		void Close();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;

		bool m_Running = true;
		bool m_Minimized = false;

		LayerStack m_LayerStack;

		float m_LastFrameTime;
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();

}