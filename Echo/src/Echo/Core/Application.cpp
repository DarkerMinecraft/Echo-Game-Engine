#include "pch.h"
#include "Application.h"

namespace Echo
{

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		EventSubject::Get()->Attach(this);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while(m_Running)
		{
			if (!m_Window->OnUpdate())
				m_Running = false;
		}
	}

	void Application::OnNotify(const Event& e)
	{
		EC_CORE_TRACE("Application::OnNotify: {0}", e.ToString());

		if (e.GetType() == EventType::WindowClose)
		{
			m_Running = false;
		}

		if (e.GetType() == EventType::WindowResize)
		{
			const WindowResizeEvent& event = static_cast<const WindowResizeEvent&>(e);
			EC_CORE_TRACE("Window resized to {0} x {1}", event.GetWidth(), event.GetHeight());
		}
	}

}