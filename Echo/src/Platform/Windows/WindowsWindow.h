#pragma once

#include "Echo/Core/Window.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Echo 
{

	class WindowsWindow : public Window 
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		virtual unsigned int GetWidth() const override;
		virtual unsigned int GetHeight() const override;

		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		virtual void* GetNativeWindow() const override;

		virtual GraphicsContext* GetContext() const override { return m_Context; }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	public:
		struct WindowData
		{
			const char* Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};
	private:
		void Init(const WindowProps& props);
		void Shutdown();
	private:
		GLFWwindow* m_Window;
		WindowData m_Data;

		GraphicsContext* m_Context;
	};

}
