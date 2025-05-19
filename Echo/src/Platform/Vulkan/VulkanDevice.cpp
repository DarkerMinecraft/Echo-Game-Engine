#include "pch.h"
#include "VulkanDevice.h"
#include "VulkanRenderCaps.h"

#include "Utils/VulkanInitializers.h"
#include "Utils/VulkanImages.h"

#include "VkBootstrap.h"
#include "VulkanSwapchain.h"

//#define VMA_DEBUG_LOG(str) EC_CORE_TRACE(str) 
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "VulkanCommandBuffer.h"

#include "VulkanFramebuffer.h"

#include <glm/glm.hpp>
#include <array>

namespace Echo 
{
	
	VulkanDevice::VulkanDevice(Window* window, unsigned int width, unsigned int height)
		: m_Window(window), m_WindowHandle((HWND)window->GetNativeWindow()), m_Width(width), m_Height(height), m_ShaderLibrary(nullptr)
	{
		InitVulkan();
		VulkanRenderCaps::Init(this);

		InitCommands();
		InitSyncStructures();
		InitSwapchain();
		CreateImGuiDescriptorPool();
		
		m_ShaderLibrary = ShaderLibrary(m_Device);
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDeviceWaitIdle(m_Device);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyCommandPool(m_Device, m_Frames[i].CommandPool, nullptr);

			vkDestroySemaphore(m_Device, m_Frames[i].SwapchainSemaphore, nullptr);
			vkDestroySemaphore(m_Device, m_Frames[i].RenderSemaphore, nullptr);
			vkDestroyFence(m_Device, m_Frames[i].RenderFence, nullptr);
		}

		vkDestroyCommandPool(m_Device, m_ImmCommandPool, nullptr);
		vkDestroyFence(m_Device, m_ImmFence, nullptr);
		vkDestroyDescriptorPool(m_Device, m_ImGuiDescriptorPool, nullptr);

		vmaDestroyAllocator(m_Allocator);
		m_Swapchain->DestroySwapchain();
		vkDestroyDevice(m_Device, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
		vkDestroyInstance(m_Instance, nullptr);
	}

	const uint32_t VulkanDevice::GetMaxTextureSlots() const
	{
		return VulkanRenderCaps::GetMaxTextureSlots();
	}

	VkImage VulkanDevice::GetSwapchainImage(uint32_t imageIndex)
	{
		return m_Swapchain->GetImage(imageIndex);
	}

	VkImageView VulkanDevice::GetSwapchainImageView(uint32_t imageIndex)
	{
		return m_Swapchain->GetImageView(imageIndex);
	}

	AllocatedBuffer VulkanDevice::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
	{
		VkBufferCreateInfo bufferInfo = { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.pNext = nullptr;
		bufferInfo.size = allocSize;

		bufferInfo.usage = usage;

		VmaAllocationCreateInfo vmaallocInfo = {};
		vmaallocInfo.usage = memoryUsage;
		vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
		AllocatedBuffer newBuffer;

		vmaCreateBuffer(m_Allocator, &bufferInfo, &vmaallocInfo, &newBuffer.Buffer, &newBuffer.Allocation, &newBuffer.Info);
		return newBuffer;
	}

	void VulkanDevice::DestroyBuffer(const AllocatedBuffer& buffer)
	{
		vmaDestroyBuffer(m_Allocator, buffer.Buffer, buffer.Allocation);
	}

	AllocatedImage VulkanDevice::CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage)
	{
		AllocatedImage newImage;
		newImage.ImageFormat = format;
		newImage.ImageExtent = size;

		VkImageCreateInfo img_info = VulkanInitializers::ImageCreateInfo(format, usage, size);

		VkSampleCountFlagBits samples = VulkanRenderCaps::GetSampleCount();
		img_info.samples = samples;

		VmaAllocationCreateInfo allocinfo = {};
		allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vmaCreateImage(m_Allocator, &img_info, &allocinfo, &newImage.Image, &newImage.Allocation, nullptr);

		VkImageAspectFlags aspectFlag;
		if (format == VK_FORMAT_D32_SFLOAT ||
			format == VK_FORMAT_D16_UNORM ||
			format == VK_FORMAT_D24_UNORM_S8_UINT ||
			format == VK_FORMAT_D32_SFLOAT_S8_UINT)
		{
			aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT)
			{
				aspectFlag |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		VkImageViewCreateInfo view_info = VulkanInitializers::ImageViewCreateInfo(format, newImage.Image, aspectFlag);
		view_info.subresourceRange.levelCount = img_info.mipLevels;

		vkCreateImageView(m_Device, &view_info, nullptr, &newImage.ImageView);
		newImage.Samples = samples;

		return newImage;
	}

	AllocatedImage VulkanDevice::CreateImageNoMSAA(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped /*= false*/)
	{
		AllocatedImage newImage;
		newImage.ImageFormat = format;
		newImage.ImageExtent = size;

		VkImageCreateInfo img_info = VulkanInitializers::ImageCreateInfo(format, usage, size);
		if (mipmapped)
		{
			img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;
		}

		img_info.samples = VK_SAMPLE_COUNT_1_BIT;

		VmaAllocationCreateInfo allocinfo = {};
		allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vmaCreateImage(m_Allocator, &img_info, &allocinfo, &newImage.Image, &newImage.Allocation, nullptr);

		VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
		if (format == VK_FORMAT_D32_SFLOAT ||
			format == VK_FORMAT_D16_UNORM ||
			format == VK_FORMAT_D24_UNORM_S8_UINT ||
			format == VK_FORMAT_D32_SFLOAT_S8_UINT)
		{
			aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT)
			{
				aspectFlag |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}

		VkImageViewCreateInfo view_info = VulkanInitializers::ImageViewCreateInfo(format, newImage.Image, aspectFlag);
		view_info.subresourceRange.levelCount = img_info.mipLevels;

		vkCreateImageView(m_Device, &view_info, nullptr, &newImage.ImageView);
		newImage.Samples = VK_SAMPLE_COUNT_1_BIT;

		return newImage;
	}

	AllocatedImage VulkanDevice::CreateImageTex(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped /*= false*/)
	{
		size_t data_size = size.depth * size.width * size.height * 4;
		AllocatedBuffer uploadbuffer = CreateBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		memcpy(uploadbuffer.Info.pMappedData, data, data_size);

		AllocatedImage newImage = CreateImage(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

		ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VulkanImages::TransitionImage(cmd, newImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = size;

			vkCmdCopyBufferToImage(cmd, uploadbuffer.Buffer, newImage.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
								   &copyRegion);

			VulkanImages::TransitionImage(cmd, newImage.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		});

		DestroyBuffer(uploadbuffer);

		return newImage;
	}

	void VulkanDevice::DestroyImage(const AllocatedImage& image)
	{
		vkDestroyImageView(m_Device, image.ImageView, nullptr);
		vmaDestroyImage(m_Allocator, image.Image, image.Allocation);
	}

	void VulkanDevice::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
	{
		vkResetFences(m_Device, 1, &m_ImmFence);
		vkResetCommandBuffer(m_ImmCommandBuffer, 0);

		VkCommandBuffer cmd = m_ImmCommandBuffer;

		VkCommandBufferBeginInfo cmdBeginInfo = VulkanInitializers::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		vkBeginCommandBuffer(cmd, &cmdBeginInfo);

		function(cmd);

		vkEndCommandBuffer(cmd);

		VkCommandBufferSubmitInfo cmdinfo = VulkanInitializers::CommandBufferSubmitInfo(cmd);
		VkSubmitInfo2 submit = VulkanInitializers::SubmitInfo(&cmdinfo, nullptr, nullptr);

		vkQueueSubmit2(m_GraphicsQueue, 1, &submit, m_ImmFence);
		vkWaitForFences(m_Device, 1, &m_ImmFence, true, UINT64_MAX);
	}

	void* VulkanDevice::GetMappedData(const AllocatedBuffer& buffer)
	{
		return buffer.Allocation->GetMappedData();
	}

	void VulkanDevice::InitVulkan()
	{
		vkb::InstanceBuilder builder;

		auto inst_ret = builder.set_app_name("Echo Engine")
			.request_validation_layers(true)
			.use_default_debug_messenger()
			.require_api_version(1, 4, 304)
			.build();

		vkb::Instance vkb_inst = inst_ret.value();

		m_Instance = vkb_inst.instance;
		m_DebugMessenger = vkb_inst.debug_messenger;

		m_Surface = m_Window->SetWindowSurface(m_Instance);

		VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
		features.dynamicRendering = true;
		features.synchronization2 = true;
		
		VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
		features12.bufferDeviceAddress = true;
		features12.descriptorIndexing = true;
		features12.runtimeDescriptorArray = true;

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.shaderStorageImageMultisample = true;
		deviceFeatures.sampleRateShading = true;
		deviceFeatures.independentBlend = true;
		deviceFeatures.robustBufferAccess = true;

		vkb::PhysicalDeviceSelector selector{ vkb_inst };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 3)
			.set_required_features_13(features)
			.set_required_features_12(features12)
			.set_required_features(deviceFeatures)
			.set_surface(m_Surface)
			.select()
			.value();

		vkb::DeviceBuilder deviceBuilder{ physicalDevice };

		vkb::Device vkbDevice = deviceBuilder.build().value();

		m_Device = vkbDevice.device;
		m_PhysicalDevice = physicalDevice.physical_device;

		m_GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		m_GraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

		m_PresentQueue = vkbDevice.get_queue(vkb::QueueType::present).value();
		m_PresentQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::present).value();

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_Device;
		allocatorInfo.instance = m_Instance;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&allocatorInfo, &m_Allocator);
	}

	void VulkanDevice::InitSwapchain()
	{
		m_Swapchain = CreateScope<VulkanSwapchain>(this, m_Width, m_Height);
		m_DrawExtent =
		{
			static_cast<uint32_t>(m_Width),
			static_cast<uint32_t>(m_Height)
		};
	}

	void VulkanDevice::RecreateSwapchain(int width, int height, VulkanSwapchain* oldSwapchain)
	{
		vkDeviceWaitIdle(m_Device);

		m_Swapchain = CreateScope<VulkanSwapchain>(this, oldSwapchain, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		m_DrawExtent = 
		{
			static_cast<uint32_t>(width), 
			static_cast<uint32_t>(height)
		};

		for (auto& fb : m_Framebuffers)
		{
			fb->UpdateSize();
		}

	}

	void VulkanDevice::InitSyncStructures()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo = VulkanInitializers::SemaphoreCreateInfo();
		VkFenceCreateInfo fenceCreateInfo = VulkanInitializers::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
		{
			vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Frames[i].RenderSemaphore);
			vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_Frames[i].SwapchainSemaphore);

			vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_Frames[i].RenderFence);
		}

		vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_ImmFence);
	}

	void VulkanDevice::InitCommands()
	{
		VkCommandPoolCreateInfo poolCreateInfo = VulkanInitializers::CommandPoolCreateInfo(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_GraphicsQueueFamily);
		
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &m_Frames[i].CommandPool);

			VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInitializers::CommandBufferAllocateInfo(m_Frames[i].CommandPool, 1);
			vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_Frames[i].CommandBuffer);
		}

		vkCreateCommandPool(m_Device, &poolCreateInfo, nullptr, &m_ImmCommandPool);

		VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInitializers::CommandBufferAllocateInfo(m_ImmCommandPool, 1);
		vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_ImmCommandBuffer);
	}

	void VulkanDevice::CreateImGuiDescriptorPool()
	{
		VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000;
		pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		vkCreateDescriptorPool(m_Device, &pool_info, nullptr, &m_ImGuiDescriptorPool);
	}

}