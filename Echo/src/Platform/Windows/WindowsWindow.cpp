#include "pch.h"
#include "WindowsWindow.h"
#include "Echo/Events/WindowEvents.h"
#include "Echo/Events/EventSubject.h"

#include <algorithm>
#include <cassert>
#include <chrono>

namespace Echo
{

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
				DestroyWindow(hwnd);
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
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

		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
		: m_HInst(GetModuleHandle(NULL)), m_Data(props.Title, props.Width, props.Height, false)
	{
		Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	bool WindowsWindow::OnUpdate()
	{
		MSG msg = {};
		while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return false;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
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

	void WindowsWindow::Init()
	{
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

		RECT rect;

		GetClientRect(GetDesktopWindow(), &rect);

		rect.left = (rect.right / 2) - (m_Data.Width / 2);
		rect.top = (rect.bottom / 2) - (m_Data.Height / 2);

		AdjustWindowRect(&rect, style, false);

		m_Window = CreateWindowEx(
			0,
			CLASS_NAME, 
			m_Data.Title,
			style,
			rect.left, rect.top, m_Data.Width, m_Data.Height,
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