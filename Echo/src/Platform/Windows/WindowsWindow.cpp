#include "pch.h"
#include "WindowsWindow.h"
#include "Echo/Events/WindowEvents.h"
#include "Echo/Events/EventSubject.h"

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

// D3D12 extension library.
#include <DirectX/d3dx12.h>

#include <algorithm>
#include <cassert>
#include <chrono>

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

	Window* Window::Create(const WindowProps& props, HINSTANCE hInst) 
	{
		return new WindowsWindow(props, hInst);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props, HINSTANCE hInst)
	{
		Init(props, hInst);
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

	void WindowsWindow::Init(const WindowProps& props, HINSTANCE hInst)	
	{
		m_Data.Title = props.Title.c_str();
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		EC_CORE_INFO("Creating window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);

		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = L"EchoEngine";


		ATOM atom = RegisterClassExW(&wc);
		EC_CORE_ASSERT(atom > 0, "Failed to register Window Class!");

		int len = strlen(m_Data.Title);
		int size_needed = MultiByteToWideChar(CP_ACP, 0, m_Data.Title, len, NULL, 0);
		WCHAR* wTitle = new WCHAR[size_needed + 1];
		MultiByteToWideChar(CP_ACP, 0, m_Data.Title, len, wTitle, size_needed);
		wTitle[size_needed] = 0;

		m_Window = CreateWindowExW(
			0,
			L"EchoWindow",
			wTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, m_Data.Width, m_Data.Height,
			NULL,
			NULL,
			hInst,
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

	ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp) 
	{

	}

}