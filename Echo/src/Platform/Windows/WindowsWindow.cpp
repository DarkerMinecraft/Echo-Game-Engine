#include "pch.h"
#include "WindowsWindow.h"

#include "Events/WindowEvents.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"

#include <cassert>
#include <chrono>

#include <windows.h>
#include <imgui.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Echo
{
	int VirtualKeyToGLFW(int vk_code)
	{
		if (vk_code == 8)
		{
			return 259;
		}
		if (vk_code == 9)
		{
			return  258;
		}
		if (vk_code == 13)
		{
			return 257;
		}

		if (vk_code == 19)
		{
			return 284;
		}
		if (vk_code == 20)
		{
			return 280;
		}
		if (vk_code == 27)
		{
			return 256;
		}
		if (vk_code == 32)
		{
			return 32;
		}
		if (vk_code == 33)
		{
			return 266;
		}
		if (vk_code == 34)
		{
			return 267;
		}
		if (vk_code == 35)
		{
			return 269;
		}
		if (vk_code == 36)
		{
			return 268;
		}
		if (vk_code == 37)
		{
			return 263;
		}
		if (vk_code == 38)
		{
			return 265;
		}
		if (vk_code == 39)
		{
			return 262;
		}
		if (vk_code == 40)
		{
			return 264;
		}
		if (vk_code == 45)
		{
			return 260;
		}
		if (vk_code == 46)
		{
			return 261;
		}
		if (vk_code >= 48 && vk_code <= 57)
		{
			return vk_code;
		}
		if (vk_code >= 65 && vk_code <= 90)
		{
			return vk_code;
		}
		if (vk_code >= 96 && vk_code <= 105)
		{
			return vk_code - 224;
		}
		if (vk_code == 106)
		{
			return 332;
		}
		if (vk_code == 107)
		{
			return 334;
		}
		if (vk_code == 109)
		{
			return 333;
		}
		if (vk_code == 110)
		{
			return 330;
		}
		if (vk_code == 111)
		{
			return 331;
		}
		if (vk_code >= 112 && vk_code <= 135)
		{
			return vk_code + 178;
		}
		if (vk_code == 144)
		{
			return 282;
		}
		if (vk_code == 145)
		{
			return 281;
		}
		if (vk_code == 160)
		{
			return 340;
		}
		if (vk_code == 161)
		{
			return 344;
		}
		if (vk_code == 162)
		{
			return 341;
		}
		if (vk_code == 163)
		{
			return 345;
		}

		if (vk_code == 164)
		{
			return 342;
		}
		return -2;
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

		if (msg == WM_CREATE)
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
			return 0;
		}

		WindowsWindow::WindowData& pData = *(reinterpret_cast<WindowsWindow::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)));
		switch (msg)
		{
			case WM_SIZE:
			{
				if (pData.StartUp)
				{
					pData.StartUp = false;
					return true;
				}

				if (wParam != SIZE_MINIMIZED)
				{
					pData.Width = LOWORD(lParam);
					pData.Height = HIWORD(lParam);
					pData.FrameBufferResized = true;

					WindowResizeEvent e(pData.Width, pData.Height);
					pData.EventCallback(e);
				}
				return true;
			}
			case WM_CLOSE:
			{
				WindowCloseEvent e;
				pData.EventCallback(e);
				return true;
			}
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				int repeatCount = (lParam & 0x0000FFFF);
				int glfwKey = VirtualKeyToGLFW(wParam);

				KeyPressedEvent event(glfwKey, repeatCount);
				pData.EventCallback(event);
				return true;
			}
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				int glfwKey = VirtualKeyToGLFW(wParam);

				KeyReleasedEvent event(glfwKey);
				pData.EventCallback(event);
				return true;
			}
			case WM_CHAR:
			{
				KeyTypedEvent event(static_cast<int>(wParam));
				pData.EventCallback(event);
				return true;
			}
			case WM_LBUTTONDOWN:
			{
				MouseButtonPressedEvent e(0); 
				pData.EventCallback(e);
				return true;
			}
			case WM_RBUTTONDOWN:
			{
				MouseButtonPressedEvent e(1); 
				pData.EventCallback(e);
				return true;
			}
			case WM_MBUTTONDOWN:
			{
				MouseButtonPressedEvent e(2);
				pData.EventCallback(e);
				return true;
			}
			case WM_LBUTTONUP:
			{
				MouseButtonReleasedEvent e(0);
				pData.EventCallback(e);
				return true;
			}
			case WM_RBUTTONUP:
			{
				MouseButtonReleasedEvent e(1);
				pData.EventCallback(e);
				return true;
			}
			case WM_MBUTTONUP:
			{
				MouseButtonReleasedEvent e(2);
				pData.EventCallback(e);
				return true;
			}
			case WM_MOUSEWHEEL:
			{
				float xOffset = 0.0f;
				float yOffset = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA);

				MouseScrolledEvent e(xOffset, yOffset);
				pData.EventCallback(e);
				return true;
			}
			case WM_MOUSEHWHEEL:
			{
				float xOffset = -static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA);
				float yOffset = 0.0f;

				MouseScrolledEvent e(xOffset, yOffset);
				pData.EventCallback(e);
				return true;
			}
			case WM_MOUSEMOVE:
			{
				POINT mousePos;
				GetCursorPos(&mousePos);

				MouseMovedEvent e(mousePos.x, mousePos.y);
				pData.EventCallback(e);
				return true;
			}
			default:
				return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
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
		MSG msg = {};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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

	void WindowsWindow::Wait()
	{
		WaitMessage();
	}

	void WindowsWindow::SetCursor(Cursor cursor)
	{
		LPCTSTR resource = nullptr;

		switch (cursor)
		{
			case Cursor::ARROW:
				resource = IDC_ARROW;
				break;
			case Cursor::RESIZE_ALL:
				resource = IDC_SIZEALL;
				break;
			case Cursor::RESIZE_NS:
				resource = IDC_SIZENS;
				break;
			case Cursor::RESIZE_EW:
				resource = IDC_SIZEWE;
				break;
			case Cursor::RESIZE_NWSE:
				resource = IDC_SIZENWSE;
				break;
			case Cursor::RESIZE_NESW:
				resource = IDC_SIZENESW;
				break;
			case Cursor::RESIZE_ROW:
				resource = IDC_SIZENS;
				break;
			case Cursor::RESIZE_COL:
				resource = IDC_SIZEWE;
				break;
			case Cursor::HAND:
				resource = IDC_HAND;
				break;
			case Cursor::NOT_ALLOWED:
				resource = IDC_NO;
				break;
			case Cursor::TEXT:
				resource = IDC_IBEAM;
				break;
			default:
				resource = IDC_ARROW;
				break;
		}

		HCURSOR hCursor = LoadCursor(NULL, resource);
		if (hCursor)
		{
			SetClassLongPtr(m_Window, GCLP_HCURSOR, (LONG_PTR)hCursor);
			::SetCursor(hCursor);
		}
	}

	VkSurfaceKHR WindowsWindow::SetWindowSurface(VkInstance instance)
	{
		EC_PROFILE_FUNCTION();
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = m_Window;
		createInfo.hinstance = GetModuleHandle(NULL);

		VkSurfaceKHR surface;
		VkResult result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
		if (result != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to create Win32 surface!");
		}

		return surface;
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
		EC_PROFILE_FUNCTION();
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title = props.Title;

		WNDCLASSEXA wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.lpszClassName = "EchoWindowClass";

		RegisterClassEx(&wc);

		if (m_Data.Width == -1 && m_Data.Height == -1)
		{
			m_Data.Width = GetSystemMetrics(SM_CXSCREEN);
			m_Data.Height = GetSystemMetrics(SM_CYSCREEN);
		}

		EC_CORE_INFO("Creating window ({0} x {1})", m_Data.Width, m_Data.Height);

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		int posX = (screenWidth - m_Data.Width) / 2;
		int posY = (screenHeight - m_Data.Height) / 2;

		RECT windowRect = { 0, 0, static_cast<LONG>(m_Data.Width), static_cast<LONG>(m_Data.Height) };
		AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW);

		m_Window = CreateWindowExA(
			WS_EX_APPWINDOW,
			"EchoWindowClass",
			m_Data.Title,
			WS_OVERLAPPEDWINDOW,
			posX,
			posY,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			GetModuleHandle(NULL),
			&m_Data 
		);

		EC_CORE_ASSERT(m_Window, "Could not create Win32 window!");
		m_Device = Device::Create(DeviceType::Vulkan, (WindowsWindow*)this, (int)m_Data.Width, (int)m_Data.Height);

		ShowWindow(m_Window, SW_SHOW);
		UpdateWindow(m_Window);
	}

	void WindowsWindow::Shutdown()
	{
		EC_PROFILE_FUNCTION();
		m_Device.reset();
		DestroyWindow(m_Window);
		UnregisterClassA("EchoWindowClass", GetModuleHandle(NULL));
	}
}