#pragma once

#include <vulkan/vulkan.h>
#include <optional>

namespace Echo
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete()
		{
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(VkInstance instance, VkSurfaceKHR surface)
			: m_Instance(instance), m_Surface(surface) {}

		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CleanUp();
		
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		VkDevice GetDevice() { return m_Device; }
	private:
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	private:
		const std::vector<const char*> m_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device;

		VkQueue m_PresentQueue;
	};
}