#pragma once

#include "Echo/Graphics/Device.h"

#include "Utils/VulkanTypes.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "vk_mem_alloc.h"
#include "Utils/VulkanDescriptors.h"
 
namespace Echo
{

	class VulkanDevice : public Device
	{
	public:
		VulkanDevice(void* window);
		virtual ~VulkanDevice();

		virtual GraphicsAPI GetGraphicsAPI() override { return GraphicsAPI::Vulkan; }
		virtual Swapchain* GetSwapchain() override { return m_Swapchain.get(); }
		virtual FrameData& GetCurrentFrame() override { return m_Frames[m_FrameNumber % FRAME_OVERLAP]; }

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void DrawBackground(Ref<Pipeline> pipeline) override;

		virtual void Start() override;
		virtual void End() override;
	public:
		VkInstance GetInstance() { return m_Instance; }
		VkDevice GetDevice() { return m_Device; }
		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		VkSurfaceKHR GetSurface() { return m_Surface; }

		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }

		VkDescriptorSetLayout GetDescriptorSetLayout() { return m_DrawImageDescriptorLayout; }
		VkDescriptorSet GetDescriptorSet() { return m_DrawImageDescriptors; }

		VkCommandBuffer GetCurrentCommandBuffer() { return m_CurrentBuffer; }

		VkImage GetCurrentImage() { return m_CurrentImage; }
		VkExtent2D GetDrawExtent() { return m_DrawExtent; }

		VmaAllocator GetAllocator() { return m_Allocator; }

		AllocatedImage GetAllocatedImage() { return m_AllocatedImage; }
		AllocatedImage GetImGuiImage() { return m_ImGuiImage; }

		uint32_t GetImageIndex() { return m_ImageIndex; }

		DeletionQueue GetDeletionQueue() { return m_DeletionQueue; }
	private:
		void InitVulkan();
		void InitSwapchain();
		void InitCommands();
		void InitSyncStructures();
		void InitDescriptors();
		void InitImGui();
	private:
		GLFWwindow* m_Window;

		DescriptorAllocator m_DescriptorAllocator;
		VkDescriptorSet m_DrawImageDescriptors;
		VkDescriptorSetLayout m_DrawImageDescriptorLayout;

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;
		VkSurfaceKHR m_Surface;

		VkQueue m_GraphicsQueue;
		uint32_t m_GraphicsQueueFamily;

		VmaAllocator m_Allocator;

		AllocatedImage m_AllocatedImage;
		AllocatedImage m_ImGuiImage;
		VkExtent2D m_DrawExtent;

		VkCommandBuffer m_CurrentBuffer;
		VkImage m_CurrentImage;
		uint32_t m_ImageIndex;

		VkClearColorValue m_ClearColorValue;

		Scope<Swapchain> m_Swapchain;
		FrameData m_Frames[FRAME_OVERLAP];
		int m_FrameNumber = 0;

		DeletionQueue m_DeletionQueue;
	};
}