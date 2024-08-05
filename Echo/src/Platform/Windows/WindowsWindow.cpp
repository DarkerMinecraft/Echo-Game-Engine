#include "pch.h"
#include "WindowsWindow.h"
#include "Echo/Events/WindowEvents.h"

#include <cassert>
#include <chrono>

namespace Echo
{

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		WindowsWindow::WindowData* pData;
		if (uMsg == WM_CREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pData = reinterpret_cast<WindowsWindow::WindowData*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pData));
		}
		else
		{
			pData = reinterpret_cast<WindowsWindow::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}

		switch (uMsg)
		{
			case WM_CLOSE:
				DestroyWindow(hwnd);
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case WM_QUIT:
				{
					WindowCloseEvent e;
					pData->EventCallback(e);
					return 0;
				}
			case WM_SIZE:
				{
					unsigned int width = LOWORD(lParam);
					unsigned int height = HIWORD(lParam);

					if(width == pData->Width && height == pData->Height)
						return 0;

					pData->Width = width;
					pData->Height = height;

					WindowResizeEvent e(width, height);
					pData->EventCallback(e);
					return 0;
				}
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
		: m_HInst(GetModuleHandle(NULL))
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		MSG msg = {};
		while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
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
		m_Data.VSync = enabled;
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
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title = props.Title;

		EC_CORE_INFO("Creating window ({0} x {1})", m_Data.Width, m_Data.Height);

		const wchar_t* CLASS_NAME = L"Echo Window Class";

		WNDCLASS wc = {};
		wc.lpszClassName = CLASS_NAME;
		wc.hInstance = m_HInst;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.lpfnWndProc = WindowProc;

		ATOM atom = RegisterClass(&wc);
		EC_CORE_ASSERT(atom > 0, "Failed to register Window Class!");

		DWORD style = WS_OVERLAPPEDWINDOW;

		RECT rect = { 0, 0, m_Data.Width, m_Data.Height };
		AdjustWindowRect(&rect, style, FALSE);

		int newWidth = rect.right - rect.left;
		int newHeight = rect.bottom - rect.top;

		RECT desktopRect;
		GetClientRect(GetDesktopWindow(), &desktopRect);

		int x = (desktopRect.right / 2) - (newWidth / 2);
		int y = (desktopRect.bottom / 2) - (newHeight / 2);

		m_Window = CreateWindowEx(
			0,
			CLASS_NAME, 
			m_Data.Title,
			style,
			x, y, newWidth, newHeight,
			NULL,
			NULL, 
			m_HInst,
			&m_Data
		);

		if (m_Window == NULL)
		{
			EC_CORE_ERROR("Failed to create window");
			return;
		}

		ShowWindow(m_Window, SW_SHOW);
	}

	void WindowsWindow::Shutdown()
	{
		const wchar_t* CLASS_NAME = L"Echo Window Class";
		
		UnregisterClass(CLASS_NAME, m_HInst);

		PostQuitMessage(0);
		DestroyWindow(m_Window);
	}

}