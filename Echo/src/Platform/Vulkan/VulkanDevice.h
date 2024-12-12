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
		virtual void DrawBackground() override;

		virtual size_t GetGPUDrawSize() override { return sizeof(GPUDrawPushConstants); }

		virtual void AddModel(Ref<Pipeline> pipeline, Ref<Model> model) override;

		virtual void DrawGeometry() override;

		virtual void Start() override;
		virtual void End() override;
		virtual void Wait() override;
	public:
		VkInstance GetInstance() { return m_Instance; }
		VkDevice GetDevice() { return m_Device; }
		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
		VkSurfaceKHR GetSurface() { return m_Surface; }

		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }

		VkDescriptorSetLayout GetDescriptorSetLayout() { return m_DrawImageDescriptorLayout; }
		VkDescriptorSet GetDescriptorSet() { return m_DrawImageDescriptors; }
		
		AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void DestroyBuffer(const AllocatedBuffer& buffer);

		GPUMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

		VkCommandBuffer GetCurrentCommandBuffer() { return m_CurrentBuffer; }

		VkImage GetCurrentImage() { return m_CurrentImage; }
		VkExtent2D GetDrawExtent() { return m_DrawExtent; }

		VmaAllocator GetAllocator() { return m_Allocator; }

		AllocatedImage GetAllocatedImage() { return m_AllocatedImage; }
		AllocatedImage GetCheckerboardImage() { return m_ErrorCheckerboardImage; }

		VkSampler GetDefaultSamplerNearest() { return m_DefaultSamplerNearest; }

		uint32_t GetImageIndex() { return m_ImageIndex; }

		AllocatedImage CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		void DestroyImage(const AllocatedImage& img);
	private:
		void InitVulkan();
		void InitSwapchain();
		void RecreateSwapchain();
		void InitCommands();
		void InitSyncStructures();
		void InitDescriptors();
		void InitDefaultData();

		void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

		AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
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
		VkExtent2D m_DrawExtent;

		VkCommandBuffer m_CurrentBuffer;
		VkImage m_CurrentImage;
		uint32_t m_ImageIndex;

		std::map<Pipeline*, std::vector<Model*>> m_Meshes;

		VkClearColorValue m_ClearColorValue;

		VkFence m_ImmFence;
		VkCommandBuffer m_ImmCommandBuffer;
		VkCommandPool m_ImmCommandPool;

		Scope<Swapchain> m_Swapchain;
		FrameData m_Frames[FRAME_OVERLAP];
		int m_FrameNumber = 0;

		AllocatedImage m_WhiteImage;
		AllocatedImage m_BlackImage;
		AllocatedImage m_GreyImage;
		AllocatedImage m_ErrorCheckerboardImage;

		VkSampler m_DefaultSamplerLinear;
		VkSampler m_DefaultSamplerNearest;
	};
}