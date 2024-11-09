#pragma once

#include "Echo/Core/Window.h"
#include "Platform/Vulkan/Interface/VulkanDevice.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

		virtual bool WasWindowResized() override { return m_Data.FrameBufferResized; };
		virtual void ResetWindowResizedFlag() override { m_Data.FrameBufferResized = false; };

		virtual Extent2D GetExtent() override { return { static_cast<uint32_t>(m_Data.Width), static_cast<uint32_t>(m_Data.Height) }; }

		virtual void Wait() override;

		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }

		virtual void* GetNativeWindow() const override;
		virtual void* GetDevice() override { return m_Device; }
	public:
		struct WindowData
		{
			const char* Title;
			unsigned int Width, Height;
			bool VSync;
			bool FrameBufferResized = false;

			EventCallbackFn EventCallback;
		};
	private:
		void Init(const WindowProps& props);
		void Shutdown();
	private:
		GLFWwindow* m_Window;
		VulkanDevice* m_Device;

		WindowData m_Data;
	};

}
