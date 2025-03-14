#pragma once

#include "Echo/Graphics/Device.h"
#include "Echo/Graphics/Image.h"

#include "vk_mem_alloc.h"
#include "Utils/VulkanTypes.h"
#include "Platform/Shader/ShaderCompiler.h"

#ifndef EC_PLATFORM_WINDOWS
	#define VK_USE_PLATFORM_WIN32_KHR
	#include <vulkan/vulkan.h>
#endif

namespace Echo
{

	struct FrameData
	{
		VkSemaphore SwapchainSemaphore, RenderSemaphore;
		VkFence RenderFence;

		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;
	};


	class VulkanSwapchain;
	class VulkanImage;

	class VulkanDevice : public Device
	{
	public:
		VulkanDevice(Window* window, unsigned int width, unsigned int height);
		~VulkanDevice();

		virtual DeviceType GetDeviceType() const { return DeviceType::Vulkan; };
		FrameData& GetFrameData() { return m_Frames[m_CurrentFrame % MAX_FRAMES_IN_FLIGHT]; }

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
		VkImageView GetSwapchainImageView(uint32_t imageIndex);

		AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void DestroyBuffer(const AllocatedBuffer& buffer);

		AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage);
		AllocatedImage CreateImageNoMSAA(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		AllocatedImage CreateImageTex(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		void DestroyImage(const AllocatedImage& image);

		void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

		void* GetMappedData(const AllocatedBuffer& buffer);
		
		void RecreateSwapchain(int width, int height, VulkanSwapchain* oldSwapchain);

		void AddImage(VulkanImage* image) { m_Images.push_back(image); }
		virtual void AddImGuiImage(Ref<Image> image) override { m_ImGuiImages.push_back(image); }
		virtual std::vector<Ref<Image>> GetImGuiImages() override { return m_ImGuiImages; }

		void AddFrame() { m_CurrentFrame++; }
	private:
		void InitVulkan();
		void InitSwapchain();
		void InitSyncStructures();
		void InitCommands();
		void CreateImGuiDescriptorPool();
	private:
		Window* m_Window;
		HWND m_WindowHandle;
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

		FrameData m_Frames[MAX_FRAMES_IN_FLIGHT];

		ShaderLibrary m_ShaderLibrary;
		uint32_t m_CurrentFrame = 0;

		std::vector<VulkanImage*> m_Images;
		std::vector<Ref<Image>> m_ImGuiImages;

		Scope<VulkanSwapchain> m_Swapchain;
	};

}