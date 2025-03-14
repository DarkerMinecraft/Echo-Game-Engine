#pragma once

#include <windows.h>

#include "Echo/Core/Window.h"

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
		virtual float GetAspectRatio() override { return (float)m_Data.Width / (float)m_Data.Height; }

		virtual bool WasWindowResized() override { return m_Data.FrameBufferResized; };
		virtual void ResetWindowResizedFlag() override { m_Data.FrameBufferResized = false; };

		virtual void Wait() override;

		virtual void SetCursor(Cursor cursor) override;
		virtual VkSurfaceKHR SetWindowSurface(VkInstance instance) override;

		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		virtual void* GetNativeWindow() const override;
		virtual Device* GetDevice() override { return m_Device.get(); }
	public:
		struct WindowData
		{
			const char* Title;
			unsigned int Width, Height;
			bool VSync;
			bool FrameBufferResized = false;
			bool StartUp = true;

			EventCallbackFn EventCallback;
		};
	private:
		void Init(const WindowProps& props);
		void Shutdown();
	private:
		HWND m_Window;
		WindowData m_Data;
		
		Scope<Device> m_Device;
	};

}
