#include "pch.h"
#include "VulkanDevice.h"
#include "Utils/VulkanInitializers.h"

#include "VkBootstrap.h"
#include "VulkanSwapchain.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace Echo 
{

	VulkanDevice::VulkanDevice(const void* window, unsigned int width, unsigned int height)
		: m_Window((GLFWwindow*)window), m_Width(width), m_Height(height)
	{
		InitVulkan();
		InitSwapchain();
		InitCommands();
		InitSyncStructures();
		CreateImGuiDescriptorPool();
		
		m_ShaderLibrary = ShaderLibrary();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDeviceWaitIdle(m_Device);

		vkDestroyImageView(m_Device, m_DrawImage.ImageView, nullptr);
		vmaDestroyImage(m_Allocator, m_DrawImage.Image, m_DrawImage.Allocation);
		vmaDestroyAllocator(m_Allocator);

		vkDestroyCommandPool(m_Device, m_ImmCommandPool, nullptr);
		vkDestroyFence(m_Device, m_ImmFence, nullptr);
		vkDestroyDescriptorPool(m_Device, m_ImGuiDescriptorPool, nullptr);

		m_Swapchain->DestroySwapchain();
		vkDestroyDevice(m_Device, nullptr);
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
		vkDestroyInstance(m_Instance, nullptr);
	}

	VkImage VulkanDevice::GetSwapchainImage(uint32_t imageIndex)
	{
		return m_Swapchain->GetImage(imageIndex);
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

		glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);

		VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
		features.dynamicRendering = true;
		features.synchronization2 = true;

		VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
		features12.bufferDeviceAddress = true;
		features12.descriptorIndexing = true;

		VkPhysicalDeviceDynamicRenderingLocalReadFeaturesKHR dynamicRenderingLocalReadFeatures{};
		dynamicRenderingLocalReadFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_LOCAL_READ_FEATURES_KHR;
		dynamicRenderingLocalReadFeatures.dynamicRenderingLocalRead = VK_TRUE;

		vkb::PhysicalDeviceSelector selector{ vkb_inst };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 3)
			.set_required_features_13(features)
			.set_required_features_12(features12)
			.add_desired_extension("VK_KHR_dynamic_rendering_local_read")
			.add_required_extension_features(dynamicRenderingLocalReadFeatures)
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
		
		VkExtent3D drawImageExtent = {
			m_Width,
			m_Height,
			1
		};

		m_DrawExtent = { m_Width, m_Height };

		m_DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		m_DrawImage.ImageExtent = drawImageExtent;

		VkImageUsageFlags drawImageUsages{};
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VkImageCreateInfo rimg_info = VulkanInitializers::ImageCreateInfo(m_DrawImage.ImageFormat, drawImageUsages, drawImageExtent);

		VmaAllocationCreateInfo rimg_allocinfo = {};
		rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vmaCreateImage(m_Allocator, &rimg_info, &rimg_allocinfo, &m_DrawImage.Image, &m_DrawImage.Allocation, nullptr);

		VkImageViewCreateInfo rview_info = VulkanInitializers::ImageViewCreateInfo(m_DrawImage.ImageFormat, m_DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);

		vkCreateImageView(m_Device, &rview_info, nullptr, &m_DrawImage.ImageView);
	}

	void VulkanDevice::RecreateSwapchain(int width, int height, VulkanSwapchain* oldSwapchain)
	{
		vkDeviceWaitIdle(m_Device);

		vkDestroyImageView(m_Device, m_DrawImage.ImageView, nullptr);
		vmaDestroyImage(m_Allocator, m_DrawImage.Image, m_DrawImage.Allocation);
		
		m_Swapchain = CreateScope<VulkanSwapchain>(this, oldSwapchain->GetSwapchain(), width, height);
		VkExtent3D drawImageExtent = {
			width,
			height,
			1
		};

		m_DrawExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		m_DrawImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		m_DrawImage.ImageExtent = drawImageExtent;

		VkImageUsageFlags drawImageUsages{};
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VkImageCreateInfo rimg_info = VulkanInitializers::ImageCreateInfo(m_DrawImage.ImageFormat, drawImageUsages, drawImageExtent);

		VmaAllocationCreateInfo rimg_allocinfo = {};
		rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vmaCreateImage(m_Allocator, &rimg_info, &rimg_allocinfo, &m_DrawImage.Image, &m_DrawImage.Allocation, nullptr);

		VkImageViewCreateInfo rview_info = VulkanInitializers::ImageViewCreateInfo(m_DrawImage.ImageFormat, m_DrawImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);

		vkCreateImageView(m_Device, &rview_info, nullptr, &m_DrawImage.ImageView);
	}

	void VulkanDevice::InitSyncStructures()
	{
		VkFenceCreateInfo fenceCreateInfo = VulkanInitializers::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_ImmFence);
	}

	void VulkanDevice::InitCommands()
	{
		VkCommandPoolCreateInfo poolCreateInfo = VulkanInitializers::CommandPoolCreateInfo(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_GraphicsQueueFamily);
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