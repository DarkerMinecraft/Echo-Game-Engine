#include "pch.h"
#include "Application.h"

#include "ImGui/ImGuiLayer.h"

#include "AssetManager/AssetRegistry.h"

namespace Echo
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const char* resourcePath, unsigned int width, unsigned int height, const char* title)
	{
		EC_CORE_ASSERT(!s_Instance, "Application already exists!");
		EC_PROFILE_FUNCTION();
		AssetRegistry::SetGlobalPath(resourcePath);
		s_Instance = this;

		WindowProps props{};
		props.Width = width;
		props.Height = height;
		props.Title = title;

		m_ImGuiLayer = new ImGuiLayer();

		m_Window = Window::Create(props);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		PushOverlay(m_ImGuiLayer);
	}

	Application::Application(const char* resourcePath, const char* title /*= "Echo Engine Game"*/)
	{
		EC_CORE_ASSERT(!s_Instance, "Application already exists!");
		EC_PROFILE_FUNCTION();
		AssetRegistry::SetGlobalPath(resourcePath);
		m_AssetWatcher = new AssetWatcher();
		s_Instance = this;

		WindowProps props{};
		props.Width = -1;
		props.Height = -1;
		props.Title = title;

		m_ImGuiLayer = new ImGuiLayer();

		m_Window = Window::Create(props);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		PushOverlay(m_ImGuiLayer);
	}

	void Application::Run()
	{
		EC_PROFILE_FUNCTION();
		Device* device = GetWindow().GetDevice();

		while(m_Running)
		{
			float time = std::chrono::duration<float>(
				std::chrono::steady_clock::now() - m_LastFrameTime
			).count();
			Timestep ts = time;
			m_LastFrameTime = std::chrono::steady_clock::now();

			if(!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(ts);

				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			} 

			m_Window->OnUpdate();
		}

		for (Layer* layer : m_LayerStack)
			layer->OnDetach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled) break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::SetImGuiBlockEvents(bool blockEvents)
	{
		m_ImGuiLayer->SetBlockEvents(blockEvents);
	}

	void Application::Close()
	{
		for (Layer* layer : m_LayerStack) 
		{
			layer->Destroy();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		delete m_AssetWatcher;
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if(e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		return false;
	}

}