#include "pch.h"
#include "VulkanSurface.h"

namespace Echo
{

	VulkanSurface::VulkanSurface(VkInstance instance, GLFWwindow* window)
		: m_Instance(instance), m_Window(window)
	{
		CreateSurface();
	}
	
	void VulkanSurface::CreateSurface()
	{
		if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to create window surface!");
		}
	}

	void VulkanSurface::CleanUp()
	{
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	}

}
