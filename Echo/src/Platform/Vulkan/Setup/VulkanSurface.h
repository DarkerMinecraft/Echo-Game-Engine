#pragma once

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

namespace Echo
{
	class VulkanSurface
	{
	public:
		VulkanSurface(VkInstance instance, GLFWwindow* window);

		void CleanUp();

		VkSurfaceKHR GetSurface() { return m_Surface; }
	private:
		void CreateSurface();
	private:
		VkInstance m_Instance;
		GLFWwindow* m_Window;

		VkSurfaceKHR m_Surface;
	};
}
