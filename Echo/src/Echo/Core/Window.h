#pragma once

#include "Base.h"
#include "Events/Event.h"

#include "Graphics/Device.h"

#ifdef ECHO_PLATFORM_WIN
	#define VK_USE_PLATFORM_WIN32_KHR
	#include <vulkan/vulkan.h>
#endif

namespace Echo 
{

	class Device;

	struct WindowProps
	{
		const char* Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const char* title = "Echo Engine", unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{}
	};

	enum Cursor 
	{
		ARROW,
		RESIZE_ALL,
		RESIZE_NS,
		RESIZE_EW,
		RESIZE_NWSE,
		RESIZE_NESW,
		RESIZE_ROW,
		RESIZE_COL,
		HAND,
		NOT_ALLOWED,
		TEXT
	};

	class Window 
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual float GetAspectRatio() = 0;

		virtual bool WasWindowResized() = 0;
		virtual void ResetWindowResizedFlag() = 0;

		virtual void SetCursor(Cursor cursor) = 0;

#ifdef ECHO_PLATFORM_WIN
		virtual VkSurfaceKHR SetWindowSurface(VkInstance instance) = 0;
#endif
		virtual void Wait() = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

		virtual void* GetNativeWindow() const = 0;
		virtual Device* GetDevice() = 0; 
	};
}
