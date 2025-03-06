#include "pch.h"
#include "WindowsWindow.h"
#include "Echo/Events/WindowEvents.h"
#include "Echo/Events/KeyEvents.h"
#include "Echo/Events/MouseEvents.h"
#include "Echo/Core/Application.h"

#include <cassert>
#include <chrono>

#include <windows.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Echo
{

	static void GLFWErrorCallback(int error, const char* description)
	{
		EC_CORE_ERROR("GLFW Error ({0}) : {1}", error, description);
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
		glfwPollEvents();
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
		glfwWaitEvents();
	}

	void WindowsWindow::SetCursor(Cursor cursor)
	{
		HWND hwnd = glfwGetWin32Window(m_Window);
		if (!hwnd) return;

		auto GetWin32CursorResource = [](Cursor resource) -> LPCTSTR
		{
			switch (resource)
			{
				case Cursor::ARROW:
					return IDC_ARROW;
				case Cursor::RESIZE_ALL:
					return IDC_SIZEALL;
				case Cursor::RESIZE_NS:
					return IDC_SIZENS;
				case Cursor::RESIZE_EW:
					return IDC_SIZEWE;
				case Cursor::RESIZE_NWSE:
					return IDC_SIZENWSE;
				case Cursor::RESIZE_NESW:
					return IDC_SIZENESW;
				case Cursor::RESIZE_ROW:
					return IDC_SIZENS;
				case Cursor::RESIZE_COL:
					return IDC_SIZEWE;
				case Cursor::HAND:
					return IDC_HAND;
				case Cursor::NOT_ALLOWED:
					return IDC_NO;
				case Cursor::TEXT:
					return IDC_IBEAM;
				default:
					return IDC_ARROW;
			}
		};

		LPCTSTR resource = GetWin32CursorResource(cursor);
		HCURSOR hCursor = LoadCursor(NULL, resource);

		if (hCursor) 
		{
			SetClassLongPtr(hwnd, GCLP_HCURSOR, (LONG_PTR)hCursor);
			::SetCursor(hCursor);
		}
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

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

		int success = glfwInit();
		EC_CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title, nullptr, nullptr);

		m_Device = Device::Create(DeviceType::Vulkan, (void*)m_Window, (int) m_Data.Width, (int) m_Data.Height);

		glfwSetWindowUserPointer(m_Window, &m_Data);

		glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			data.FrameBufferResized = true;

			WindowResizeEvent e(width, height);
			data.EventCallback(e);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					data.EventCallback(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent e(button);
					data.EventCallback(e);
					break;
				}
			}

		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xOffset, (float)yOffset);
			data.EventCallback(e);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)xPos, (float)yPos);
			data.EventCallback(e);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent e;
			data.EventCallback(e);
		});
	}

	void WindowsWindow::Shutdown()
	{
		m_Device.reset();
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

}
