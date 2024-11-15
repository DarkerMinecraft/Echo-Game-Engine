#pragma once

#include "Echo/Graphics/Device.h"

#include "vulkan/vulkan.h"

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

	class VulkanDevice : public Device 
	{
	public:
		VulkanDevice(void* hwnd, const GraphicsDeviceCreateInfo& createInfo);
		virtual ~VulkanDevice(); 

		virtual const API GetGraphicsAPI() const override { return API::Vulkan; }

		virtual Swapchain* GetSwapchain() override { return m_Swapchain.get(); }
		virtual CommandBuffer* GetCommandBuffer() override { return m_CommandBuffer.get(); }

		virtual void AddMesh(Ref<Resource> graphicsPipeline, Vertex vertex);
	public:
		VkSurfaceKHR GetSurface() { return m_Surface; }
		VkDevice GetDevice() { return m_Device; }

		VkFormat ConvertToVulkanFormat(ImageFormat format);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device = nullptr);
	private:
		void CreateInstance();
		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();

		void SetupDebugMessenger();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

		void CreateSurface();

		void PickPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		void CreateLogicalDevice();

		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		void CreateSwapchain();

		void CreateCommandPool();
	private:
		const GraphicsDeviceCreateInfo m_DeviceInfo; 
		void* m_Hwnd;

		Scope<Swapchain> m_Swapchain;
		Scope<CommandBuffer> m_CommandBuffer;

		std::map<Ref<Resource>, Vertex> m_Meshes; 

		const std::vector<const char*> m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> m_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device;
		VkSurfaceKHR m_Surface;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		VkCommandPool m_CommandPool;
	};

}