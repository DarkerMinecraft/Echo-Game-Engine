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
		Application();
		virtual ~Application();

		void Run();

		virtual void OnNotify(const Event& e) override;
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	Application* CreateApplication();

}