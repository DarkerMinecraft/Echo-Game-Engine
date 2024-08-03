#pragma once

#include "Window.h"
#include "Echo/Events/Observer.h"
#include "Echo/Events/Event.h"
#include "Echo/Events/EventSubject.h"
#include "Echo/Events/WindowEvents.h"

namespace Echo
{

	class Application : public Observer
	{
	public:
#ifdef ECHO_PLATFORM_WIN
		Application(HINSTANCE hInst);
#elif
		Application();
#endif
		virtual ~Application();

		void Run();

		virtual void OnNotify(const Event& e) override;
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

#ifdef ECHO_PLATFORM_WIN
	Application* CreateApplication(HINSTANCE hINst);
#elif
	Application* CreateApplication();
#endif

}