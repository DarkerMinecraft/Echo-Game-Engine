#include "pch.h"
#include "WindowsWindow.h"
#include "Echo/Events/WindowEvents.h"
#include "Echo/Events/EventSubject.h"

#include <windows.h>

namespace Echo 
{

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowsWindow::WindowData* pData;
		if (uMsg == WM_CREATE)
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			pData = (WindowsWindow::WindowData*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pData);
		}
		else
		{
			pData = (WindowsWindow::WindowData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		switch (uMsg)
		{
			case WM_CLOSE:
				EventSubject::Get()->Notify(WindowCloseEvent());
				return 0;
			case WM_SIZE:
			{
				unsigned int width = LOWORD(lParam);
				unsigned int height = HIWORD(lParam);
				pData->Width = width;
				pData->Height = height;

				EventSubject::Get()->Notify(WindowResizeEvent(width, height));
				return 0;
			}
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	Window* Window::Create(const WindowProps& props) 
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	unsigned int WindowsWindow::GetWidth() const
	{
		return m_Data.Width;
	}

	unsigned int WindowsWindow::GetHeight() const 
	{
		return m_Data.Height;
	}

	void WindowsWindow::SetVSync(bool enabled) 
	{

	}

	bool WindowsWindow::IsVSync() const 
	{
		return m_Data.VSync;
	}

	void* WindowsWindow::GetNativeWindow() const 
	{
		return m_Window;
	}

	void WindowsWindow::Init(const WindowProps& props)	
	{
		m_Data.Title = props.Title.c_str();
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		EC_CORE_INFO("Creating window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);

		WNDCLASS wc = {};
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = L"EchoWindow";

		RegisterClass(&wc);

		int len = strlen(m_Data.Title);
		int size_needed = MultiByteToWideChar(CP_ACP, 0, m_Data.Title, len, NULL, 0);
		WCHAR* wTitle = new WCHAR[size_needed + 1];
		MultiByteToWideChar(CP_ACP, 0, m_Data.Title, len, wTitle, size_needed);
		wTitle[size_needed] = 0;

		m_Window = CreateWindowEx(
			0,
			L"EchoWindow",
			wTitle,
			WS_OVERLAPPEDWINDOW,

			CW_USEDEFAULT, CW_USEDEFAULT, m_Data.Width, m_Data.Height,
			NULL, 
			NULL, 
			GetModuleHandle(NULL),
			&m_Data
		);

		if(m_Window == NULL)
		{
			EC_CORE_ERROR("Failed to create window");
			return;
		}

		ShowWindow(m_Window, SW_SHOW);
	}

	void WindowsWindow::Shutdown()
	{
		PostQuitMessage(0);
		DestroyWindow(m_Window);
	}

}