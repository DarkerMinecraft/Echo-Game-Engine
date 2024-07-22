#pragma once

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

		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void* GetNativeWindow() const override;
	public:
		struct WindowData
		{
			const char* Title;
			unsigned int Width, Height;
			bool VSync;
		};
	private:
		void Init(const WindowProps& props);
		void Shutdown();
	private:
		HWND m_Window;
		WindowData m_Data;
	};

}