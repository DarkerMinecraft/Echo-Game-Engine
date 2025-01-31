#pragma once

#include "Echo/Graphics/Device.h"
#include "Echo/ImGui/ImGuiLayer.h"
#include "Platform/Shader/ShaderCompiler.h"
#include "Utils/VulkanTypes.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <deque>

namespace Echo
{

	class VulkanSwapchain;

	struct VulkanFrameData
	{
		VkSemaphore SwapchainSemaphore, RenderSemaphore;
		VkFence RenderFence;

		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;
	};

	class VulkanDevice : public Device
	{
	public:
		VulkanDevice(void* windowHwnd, int width, int height);
		virtual ~VulkanDevice();

		virtual void Start() override;
		virtual void End() override;

		virtual Ref<Buffer> CreateBuffer(const BufferDesc& bufferDescription) override;
		virtual Ref<Texture> CreateTexture(const TextureDesc& textureDescription) override;
		virtual Ref<Pipeline> CreatePipeline(const PipelineDesc& pipelineDescription) override;
		virtual Ref<FrameBuffer> CreateFrameBuffer(const FrameBufferDesc& frameBufferDescription) override;

		virtual Ref<Mesh> CreateMesh(std::vector<Vertex3D> meshData, std::vector<uint32_t> indices) override;
		virtual Ref<Mesh> CreateMesh(std::vector<Vertex2D> meshData, std::vector<uint32_t> indices) override;

		virtual void CMDDispatch(float groupXScale, float groupYScale) override;
		virtual void CMDDrawIndexed(uint32_t indicesSize) override;
	public:
		VkInstance GetInstance() { return m_Instance; }
		VkDevice GetDevice() { return m_Device; }
		VkSurfaceKHR GetSurface() { return m_Surface; }
		VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }

		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		VkDescriptorPool GetImGuiDescriptorPool() { return m_ImGuiPool; }

		VulkanSwapchain* GetSwapchain() { return m_Swapchain.get(); }

		VmaAllocator GetAllocator() { return m_Allocator; }

		VkCommandBuffer GetActiveCommandBuffer() { return m_ActiveCommandBuffer; }

		AllocatedImage GetActiveImage() { return m_ActiveImage; }
		Ref<Texture> GetDrawImage() { return m_DrawImage; }

		VulkanFrameData& GetCurrentFrame() { return m_FramesData[m_FrameNumber % FRAME_OVERLAP]; }

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void DestroyBuffer(const AllocatedBuffer& buffer);

		AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		AllocatedImage CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
		void DestroyImage(const AllocatedImage& img);

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer cmd);
	private:
		void CreateSwapchain(int width, int height);
		void RecreateSwapchain(int width, int height, VulkanSwapchain* oldSwapchain);

		void Initalize(void* windowHwnd);
		void InitSyncStructures();
		void InitCommands();
		void CreateImGuiDescriptorPool();

		void CreateSingleUseCommandPool();

		void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);		
	private:
		Scope<VulkanSwapchain> m_Swapchain;

		ShaderLibrary m_ShaderLibrary;
		
		VkDescriptorPool m_ImGuiPool;
		VkCommandPool m_SingleUseCommandPool;

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkSurfaceKHR m_Surface;

		VkPhysicalDevice m_PhysicalDevice;
		VkDevice m_Device;

		VkQueue m_GraphicsQueue;
		uint32_t m_GraphicsQueueFamily;

		VkCommandPool m_ImmCommandPool;
		VkCommandBuffer m_ImmCommandBuffer;
		VkFence m_ImmFence;

		VkCommandBuffer m_ActiveCommandBuffer;
		AllocatedImage m_ActiveImage;

		VulkanFrameData m_FramesData[FRAME_OVERLAP];

		uint32_t m_ImageIndex;
		int m_FrameNumber = 0;

		VkExtent2D m_DrawExtent;

		VmaAllocator m_Allocator;

		Ref<Texture> m_DrawImage;

		std::vector<Ref<Pipeline>> m_Pipelines;
		std::vector<Ref<Texture>> m_Textures;
		std::vector<Ref<Buffer>> m_Buffers;
		std::vector<Ref<FrameBuffer>> m_FrameBuffers;
		std::vector<Ref<Mesh>> m_Meshes;
	};

}