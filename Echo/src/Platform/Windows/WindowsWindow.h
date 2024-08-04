#pragma once

#include "Echo/Core/Window.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Echo 
{

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	class WindowsWindow : public Window 
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual bool OnUpdate() override;

		virtual unsigned int GetWidth() const override;
		virtual unsigned int GetHeight() const override;

		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void* GetNativeWindow() const override;
	public:
		struct WindowData
		{
			const wchar_t* Title;
			unsigned int Width, Height;
			bool VSync;

			WindowData(const wchar_t* title, unsigned int width, unsigned int height, bool vsync)
				: Title(title), Width(width), Height(height), VSync(vsync)
			{}
		};
	private:
		void Init();
		void Shutdown();
	private:
		HINSTANCE m_HInst;
		HWND m_Window;
		WindowData m_Data;
	};

}