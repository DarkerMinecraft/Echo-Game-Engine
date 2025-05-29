#pragma once

#include "LayerStack.h"
#include "Window.h"

#include "Events/Event.h"
#include "Events/WindowEvents.h"
#include "Log.h"

#include "AssetManager/AssetWatcher.h"

#include <chrono>

namespace Echo
{

	class ImGuiLayer;

	class Application
	{
	public:
		Application(const char* resourcePath, unsigned int width = 1280, unsigned int height = 720, const char* title = "Echo Engine Game");
		Application(const char* resourcePath, const char* title = "Echo Engine Game");
		virtual ~Application() = default;

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static Application& Get() { return *s_Instance; }
		Window& GetWindow() { return *m_Window; }

		void SetImGuiBlockEvents(bool blockEvents); 

		void Close();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Scope<Window> m_Window;

		bool m_Running = true;
		bool m_Minimized = false;

		ImGuiLayer* m_ImGuiLayer;

		AssetWatcher* m_AssetWatcher;

		LayerStack m_LayerStack;

		std::chrono::steady_clock::time_point m_LastFrameTime;
	private:
		static Application* s_Instance;
	};

	Application* CreateApplication();

}