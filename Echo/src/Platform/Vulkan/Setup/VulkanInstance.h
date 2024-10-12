#pragma once

#include <vulkan/vulkan.h>

namespace Echo
{
	class VulkanInstance
	{
	public:
		VulkanInstance();

		void SetupDebugMessenger();
		void CleanUp();

		VkInstance GetInstance() const { return m_Instance; }
	private:
		void CreateInstance();

		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	private:
		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		const std::vector<const char*> m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

#ifdef NDEBUG
		const bool m_EnableValidationLayers = false;
#else
		const bool m_EnableValidationLayers = true;
#endif
	};
}
