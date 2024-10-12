#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <GLFW/glfw3.h>
#include "VulkanDevice.h"

namespace Echo
{
	class VulkanSwapChain
	{
	public:
		VulkanSwapChain(VkInstance instance, VkSurfaceKHR surface, GLFWwindow* window, VulkanDevice* device)
			: m_Instance(instance), m_Surface(surface), m_Window(window), m_Device(device) {}

		void CreateSwapChain();
		void CreateImageViews();
		void CleanUp();

		VkSwapchainKHR GetSwapChain() { return m_SwapChain; }
		std::vector<VkImage> GetSwapChainImages() { return m_SwapChainImages; }

		VkExtent2D GetSwapChainExtent() { return m_SwapChainExtent; }
		VkFormat GetSwapChainImageFormat() { return m_SwapChainImageFormat; }
	private:
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	private:
		GLFWwindow* m_Window;
		VulkanDevice* m_Device;

		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		VkSwapchainKHR m_SwapChain;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
	};
}