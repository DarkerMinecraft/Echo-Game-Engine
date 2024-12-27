#include "pch.h"
#include "WindowsWindow.h"
#include "Echo/Events/WindowEvents.h"

#include <cassert>
#include <chrono>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

		m_Device = Device::Create(GraphicsAPI::Vulkan, (void*)m_Window, (int) m_Data.Width, (int) m_Data.Height);

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
