#pragma once

#include "Base.h"

#include "Echo/Events/Event.h"

namespace Echo 
{

#ifdef ECHO_PLATFORM_WIN
	struct WindowProps 
	{
		const wchar_t* Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const wchar_t* title = L"Echo Engine", unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height) {}
	};
#else 
	struct WindowProps
	{
		const std::string& Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string title = "Echo Engine", unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{}
	};
#endif

	class Window 
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		static Window* Create(const WindowProps& props = WindowProps());
		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		virtual void* GetNativeWindow() const = 0;
	};
}