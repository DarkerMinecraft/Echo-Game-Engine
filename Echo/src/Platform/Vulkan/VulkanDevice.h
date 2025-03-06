#pragma once

#include "Echo/Graphics/Device.h"

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include "vk_mem_alloc.h"
#include "Utils/VulkanTypes.h"
#include <Platform/Shader/ShaderCompiler.h>

namespace Echo
{

	class VulkanSwapchain;

	class VulkanDevice : public Device
	{
	public:
		VulkanDevice(const void* window, unsigned int width, unsigned int height);
		~VulkanDevice();

		virtual DeviceType GetDeviceType() const { return DeviceType::Vulkan; };

		ShaderLibrary GetShaderLibrary() { return m_ShaderLibrary; }

		VkInstance GetInstance() { return m_Instance; }
		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		VkQueue GetPresentQueue() { return m_PresentQueue; }
		VkDevice GetDevice() { return m_Device; }
		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		VkSurfaceKHR GetSurface() { return m_Surface; }

		VkDescriptorPool GetImGuiDescriptorPool() { return m_ImGuiDescriptorPool; }
		
		uint32_t GetGraphicsQueueFamily() { return m_GraphicsQueueFamily; }
		uint32_t GetPresentQueueFamily() { return m_PresentQueueFamily; }

		VulkanSwapchain& GetSwapchain() { return *m_Swapchain; }

		VmaAllocator GetAllocator() { return m_Allocator; }

		VkExtent2D GetDrawExtent() { return m_DrawExtent; }
		AllocatedImage GetDrawImage() { return m_DrawImage; }

		VkImage GetSwapchainImage(uint32_t imageIndex);

		AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void DestroyBuffer(const AllocatedBuffer& buffer);

		void DestroyImage(const AllocatedImage& image);

		void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

		void* GetMappedData(const AllocatedBuffer& buffer);
		
		void RecreateSwapchain(int width, int height, VulkanSwapchain* oldSwapchain);
	private:
		void InitVulkan();
		void InitSwapchain();
		void InitSyncStructures();
		void InitCommands();
		void CreateImGuiDescriptorPool();
	private:
		GLFWwindow* m_Window;
		unsigned int m_Width;
		unsigned int m_Height;

		VkInstance m_Instance;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;
		VkQueue m_GraphicsQueue;
		uint32_t m_GraphicsQueueFamily;
		VkQueue m_PresentQueue;
		uint32_t m_PresentQueueFamily;
		VkSurfaceKHR m_Surface;
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		VkFence m_ImmFence;
		VkCommandBuffer m_ImmCommandBuffer;
		VkCommandPool m_ImmCommandPool;

		VkDescriptorPool m_ImGuiDescriptorPool;
		
		VmaAllocator m_Allocator;
		AllocatedImage m_DrawImage;
		VkExtent2D m_DrawExtent;

		ShaderLibrary m_ShaderLibrary;

		Scope<VulkanSwapchain> m_Swapchain;
	};

}