#include "pch.h"
#include "VulkanDevice.h"

#include "Echo/Core/Application.h"

#include "Utils/VulkanInitializers.h"

#include <VkBootstrap.h>
#include <GLFW/glfw3.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "VulkanFrameBuffer.h"
#include "VulkanSwapchain.h"

#include "Utils/VulkanImages.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanTexture.h"
#include "VulkanMesh.h"

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

namespace Echo 
{

	VulkanDevice::VulkanDevice(void* windowHwnd, int width, int height)
		: m_ShaderLibrary(ShaderLibrary())
	{
		Initalize(windowHwnd);
		CreateSwapchain(width, height);
		InitSyncStructures();
		InitCommands();	
		CreateImGuiDescriptorPool();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDeviceWaitIdle(m_Device);
		vkQueueWaitIdle(m_GraphicsQueue);
		
		for (int i = 0; i < FRAME_OVERLAP; i++)
		{
			vkWaitForFences(m_Device, 1, &m_FramesData[i].RenderFence, VK_TRUE, UINT64_MAX);

			vkDestroyFence(m_Device, m_FramesData[i].RenderFence, nullptr);
			vkDestroySemaphore(m_Device, m_FramesData[i].SwapchainSemaphore, nullptr);
			vkDestroySemaphore(m_Device, m_FramesData[i].RenderSemaphore, nullptr);

			vkFreeCommandBuffers(m_Device, m_FramesData[i].CommandPool, 1, &m_FramesData[i].CommandBuffer);
			vkDestroyCommandPool(m_Device, m_FramesData[i].CommandPool, nullptr);
		}

		vkWaitForFences(m_Device, 1, &m_ImmFence, true, UINT64_MAX);
		vkDestroyFence(m_Device, m_ImmFence, nullptr);

		vkFreeCommandBuffers(m_Device, m_ImmCommandPool, 1, &m_ImmCommandBuffer);
		vkDestroyCommandPool(m_Device, m_ImmCommandPool, nullptr);

		vkDestroyDescriptorPool(m_Device, m_ImGuiPool, nullptr);

		m_Pipelines.clear();
		m_Textures.clear();
		m_Buffers.clear();
		m_FrameBuffers.clear();

		m_DrawImage.reset();
		m_Swapchain.reset();

		vmaDestroyAllocator(m_Allocator);

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanDevice::Start()
	{
		vkDeviceWaitIdle(m_Device);

		AllocatedImage drawImage = ((VulkanTexture*)m_DrawImage.get())->GetAllocatedImage();
		m_DrawExtent.width = drawImage.ImageExtent.width;
		m_DrawExtent.height = drawImage.ImageExtent.height;

		vkWaitForFences(m_Device, 1, &GetCurrentFrame().RenderFence, true, UINT64_MAX);
		vkResetFences(m_Device, 1, &GetCurrentFrame().RenderFence);

		m_ImageIndex = m_Swapchain->AcquireNextImage();
		if (m_ImageIndex == -1)
		{
			RecreateSwapchain(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), m_Swapchain.get());
			return;
		}

		m_ActiveCommandBuffer = GetCurrentFrame().CommandBuffer;

		vkResetCommandBuffer(m_ActiveCommandBuffer, 0);

		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.pNext = nullptr;

		info.pInheritanceInfo = nullptr;
		info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(m_ActiveCommandBuffer, &info) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		m_ActiveImage = m_Swapchain->GetSwapchainImage(m_ImageIndex);

		VulkanImages::TransitionImage(m_ActiveCommandBuffer, drawImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanDevice::End()
	{
		AllocatedImage drawImage = ((VulkanTexture*)m_DrawImage.get())->GetAllocatedImage();

		VulkanImages::TransitionImage(m_ActiveCommandBuffer, m_ActiveImage.Image,
									  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VulkanImages::TransitionImage(m_ActiveCommandBuffer, drawImage.Image,
									  VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

		VulkanImages::CopyImageToImage(m_ActiveCommandBuffer, drawImage.Image,
									   m_ActiveImage.Image, m_DrawExtent, m_Swapchain->GetExtent());

		VulkanImages::TransitionImage(m_ActiveCommandBuffer, m_ActiveImage.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		Application::Get().GetImGuiLayer()->DrawImGui();
		VulkanImages::TransitionImage(m_ActiveCommandBuffer, m_ActiveImage.Image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		if (vkEndCommandBuffer(m_ActiveCommandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end recording command buffer");
		}

		VkCommandBufferSubmitInfo cmdInfo = VulkanInitializers::CommandBufferSubmitInfo(m_ActiveCommandBuffer);
		VkSemaphore renderWaitSemaphore = GetCurrentFrame().SwapchainSemaphore;
		VkSemaphoreSubmitInfo renderWaitInfo = VulkanInitializers::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, renderWaitSemaphore);
		VkSemaphore renderSignalSemaphore = GetCurrentFrame().RenderSemaphore;
		VkSemaphoreSubmitInfo renderSignalInfo = VulkanInitializers::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, renderSignalSemaphore);

		VkSubmitInfo2 submit = VulkanInitializers::SubmitInfo(&cmdInfo, &renderSignalInfo, &renderWaitInfo);
		VkFence renderFence = GetCurrentFrame().RenderFence;

		vkQueueSubmit2(m_GraphicsQueue, 1, &submit, renderFence);

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		VkSwapchainKHR swapchain = ((VulkanSwapchain*)m_Swapchain.get())->GetSwapchain();
		presentInfo.pSwapchains = &swapchain;
		presentInfo.swapchainCount = 1;
		presentInfo.pWaitSemaphores = &renderSignalSemaphore;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pImageIndices = &m_ImageIndex;

		VkResult presentResult = vkQueuePresentKHR(m_GraphicsQueue, &presentInfo);

		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR || Application::Get().GetWindow().WasWindowResized())
		{
			Application::Get().GetWindow().ResetWindowResizedFlag();
			RecreateSwapchain(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight(), m_Swapchain.get());
		}
		else if (presentResult != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image");
		}

		m_FrameNumber++;
	}

	uint32_t VulkanDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;  
			}
		}

		throw std::runtime_error("Failed to find suitable memory type!");
	}

	Ref<Buffer> VulkanDevice::CreateBuffer(const BufferDesc& bufferDescription)
	{
		Ref<Buffer> buffer = CreateRef<VulkanBuffer>(this, bufferDescription);
		m_Buffers.push_back(buffer);

		return buffer;
	}

	Ref<Texture> VulkanDevice::CreateTexture(const TextureDesc& textureDescription)
	{
		Ref<Texture> texture = CreateRef<VulkanTexture>(this, textureDescription);
		m_Textures.push_back(texture);

		return texture;
	}

	Ref<Pipeline> VulkanDevice::CreatePipeline(const PipelineDesc& pipelineDescription)
	{
		Ref<Pipeline> pipeline = CreateRef<VulkanPipeline>(this, m_ShaderLibrary, pipelineDescription);
		m_Pipelines.push_back(pipeline);

		return pipeline;
	}

	Ref<FrameBuffer> VulkanDevice::CreateFrameBuffer(const FrameBufferDesc& frameBufferDescription)
	{
		Ref<FrameBuffer> frameBuffer = CreateRef<VulkanFrameBuffer>(this, frameBufferDescription);
		m_FrameBuffers.push_back(frameBuffer);

		return frameBuffer;
	}

	Ref<Mesh> VulkanDevice::CreateMesh(std::vector<Vertex3D> meshData, std::vector<uint32_t> indices)
	{
		Ref<Mesh> mesh = CreateRef<VulkanMesh>(this, meshData, indices);
		m_Meshes.push_back(mesh);

		return mesh;
	}

	Ref<Mesh> VulkanDevice::CreateMesh(std::vector<Vertex2D> meshData, std::vector<uint32_t> indices)
	{
		Ref<Mesh> mesh = CreateRef<VulkanMesh>(this, meshData, indices);
		m_Meshes.push_back(mesh);

		return mesh;
	}

	void VulkanDevice::CMDDispatch(float groupXScale, float groupYScale)
	{
		vkCmdDispatch(m_ActiveCommandBuffer, std::ceil(m_DrawExtent.width * groupXScale), std::ceil(m_DrawExtent.height * groupYScale), 1);
	}

	void VulkanDevice::CMDDrawIndexed(uint32_t indicesSize)
	{
		vkCmdDrawIndexed(m_ActiveCommandBuffer, indicesSize, 1, 0, 0, 0);
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

		vmaCreateBuffer(m_Allocator, &bufferInfo, &vmaallocInfo, &newBuffer.Buffer, &newBuffer.Allocation,
						&newBuffer.Info);

		return newBuffer;
	}

	void VulkanDevice::DestroyBuffer(const AllocatedBuffer& buffer)
	{
		vmaDestroyBuffer(m_Allocator, buffer.Buffer, buffer.Allocation);
	}

	AllocatedImage VulkanDevice::CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped /*= false*/)
	{
		AllocatedImage newImage;
		newImage.ImageFormat = format;
		newImage.ImageExtent = size;

		VkImageCreateInfo img_info = VulkanInitializers::ImageCreateInfo(format, usage, size);
		if (mipmapped)
		{
			img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;
		}

		VmaAllocationCreateInfo allocinfo = {};
		allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vmaCreateImage(m_Allocator, &img_info, &allocinfo, &newImage.Image, &newImage.Allocation, nullptr) != VK_SUCCESS)
		{
			//Error
		}

		VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
		if (format == VK_FORMAT_D32_SFLOAT)
		{
			aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		VkImageViewCreateInfo view_info = VulkanInitializers::ImageViewCreateInfo(format, newImage.Image, aspectFlag);
		view_info.subresourceRange.levelCount = img_info.mipLevels;

		if (vkCreateImageView(m_Device, &view_info, nullptr, &newImage.ImageView) != VK_SUCCESS)
		{
			//Error
		}

		return newImage;
	}


	AllocatedImage VulkanDevice::CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped /*= false*/)
	{
		size_t data_size = size.depth * size.width * size.height * 4;
		AllocatedBuffer uploadbuffer = CreateBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		memcpy(uploadbuffer.Info.pMappedData, data, data_size);

		AllocatedImage new_image = CreateImage(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);

		ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VulkanImages::TransitionImage(cmd, new_image.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;

			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = size;

			vkCmdCopyBufferToImage(cmd, uploadbuffer.Buffer, new_image.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
								   &copyRegion);

			VulkanImages::TransitionImage(cmd, new_image.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
										  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		});

		DestroyBuffer(uploadbuffer);

		return new_image;
	}

	void VulkanDevice::DestroyImage(const AllocatedImage& img)
	{
		vkDestroyImageView(m_Device, img.ImageView, nullptr);
		vmaDestroyImage(m_Allocator, img.Image, img.Allocation);
	}

	VkCommandBuffer VulkanDevice::BeginSingleTimeCommands()
	{
		CreateSingleUseCommandPool();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_SingleUseCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	void VulkanDevice::EndSingleTimeCommands(VkCommandBuffer cmd)
	{
		vkEndCommandBuffer(cmd);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmd;

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueue);

		vkFreeCommandBuffers(m_Device, m_SingleUseCommandPool, 1, &cmd);
		vkDestroyCommandPool(m_Device, m_SingleUseCommandPool, nullptr);
	}

	void VulkanDevice::Initalize(void* windowHwnd)
	{
		vkb::InstanceBuilder builder;

		auto inst_ret = builder.set_app_name("Echo Engine Application")
			.request_validation_layers(true)
			.use_default_debug_messenger()
			.require_api_version(1, 3, 0)
			.build();

		vkb::Instance vkb_inst = inst_ret.value();

		m_Instance = vkb_inst.instance;
		m_DebugMessenger = vkb_inst.debug_messenger;
		
		glfwCreateWindowSurface(m_Instance, (GLFWwindow*)windowHwnd, nullptr, &m_Surface);

		VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
		features.dynamicRendering = true;
		features.synchronization2 = true;

		VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
		features12.bufferDeviceAddress = true;
		features12.descriptorIndexing = true;

		vkb::PhysicalDeviceSelector selector{ vkb_inst };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 3)
			.set_required_features_13(features)
			.set_required_features_12(features12)
			.add_desired_extension("VK_KHR_multiview")
			.add_desired_extension("VK_KHR_dynamic_rendering")
			.set_surface(m_Surface)
			.select()
			.value();

		vkb::DeviceBuilder deviceBuilder{ physicalDevice };

		vkb::Device vkbDevice = deviceBuilder.build().value();

		m_Device = vkbDevice.device;
		m_PhysicalDevice = physicalDevice.physical_device;

		m_GraphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
		m_GraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_Device;
		allocatorInfo.instance = m_Instance;
		allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&allocatorInfo, &m_Allocator);
	}

	void VulkanDevice::CreateSwapchain(int width, int height)
	{
		m_Swapchain = CreateScope<VulkanSwapchain>(this, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		TextureDesc drawImageDesc{};
		drawImageDesc.Format = TextureFormat::RGBA16;
		drawImageDesc.Width = static_cast<uint32_t>(width);
		drawImageDesc.Height = static_cast<uint32_t>(height);
		drawImageDesc.Usage = TextureUsage::TransferSrc | TextureUsage::TransferDst | TextureUsage::Storage | TextureUsage::ColorAttachment;

		m_DrawImage = CreateRef<VulkanTexture>(this, drawImageDesc);
	}

	void VulkanDevice::RecreateSwapchain(int width, int height, VulkanSwapchain* oldSwapchain)
	{
		vkDeviceWaitIdle(m_Device);

		m_DrawImage.reset();
		
		m_Swapchain = CreateScope<VulkanSwapchain>(this, static_cast<uint32_t>(width), static_cast<uint32_t>(height), oldSwapchain);

		TextureDesc drawImageDesc{};
		drawImageDesc.Format = TextureFormat::RGBA16;
		drawImageDesc.Width = static_cast<uint32_t>(width);
		drawImageDesc.Height = static_cast<uint32_t>(height);
		drawImageDesc.Usage = TextureUsage::TransferSrc | TextureUsage::TransferDst | TextureUsage::Storage | TextureUsage::ColorAttachment;

		m_DrawImage = CreateRef<VulkanTexture>(this, drawImageDesc);

		for (auto& texture : m_Textures)
		{
			VulkanTexture* tex = (VulkanTexture*)texture.get();

			tex->UpdateSwapchainExtent(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		}

		for (auto& frameBuffer : m_FrameBuffers)
		{
			VulkanFrameBuffer* fb = (VulkanFrameBuffer*)frameBuffer.get();

			fb->UpdateSwapchain(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		}
	}

	void VulkanDevice::InitSyncStructures()
	{
		for (int i = 0; i < FRAME_OVERLAP; i++) 
		{
			VkSemaphoreCreateInfo semaphoreInfo = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
			vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_FramesData[i].RenderSemaphore);
			vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_FramesData[i].SwapchainSemaphore);

			VkFenceCreateInfo fenceInfo = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT };
			vkCreateFence(m_Device, &fenceInfo, nullptr, &m_FramesData[i].RenderFence);
		}

		VkFenceCreateInfo fenceInfo = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT };
		vkCreateFence(m_Device, &fenceInfo, nullptr, &m_ImmFence);
	}

	void VulkanDevice::InitCommands()
	{
		for (int i = 0; i < FRAME_OVERLAP; i++)
		{

			VkCommandPoolCreateInfo commandPoolInfo = {};
			commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolInfo.pNext = nullptr;
			commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolInfo.queueFamilyIndex = m_GraphicsQueueFamily;

			if (vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_FramesData[i].CommandPool) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create command pool");
			}
			
			VkCommandBufferAllocateInfo cmdAllocInfo = {};
			cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			cmdAllocInfo.pNext = nullptr;
			cmdAllocInfo.commandPool = m_FramesData[i].CommandPool;
			cmdAllocInfo.commandBufferCount = 1;
			cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

			if (vkAllocateCommandBuffers(m_Device, &cmdAllocInfo, &m_FramesData[i].CommandBuffer) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate command buffers");
			}
		}

		VkCommandPoolCreateInfo commandPoolInfo = {};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.pNext = nullptr;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = m_GraphicsQueueFamily;

		vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_ImmCommandPool);
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

		vkCreateDescriptorPool(m_Device, &pool_info, nullptr, &m_ImGuiPool);
	}

	void VulkanDevice::CreateSingleUseCommandPool()
	{
		VkCommandPoolCreateInfo commandPoolInfo = {};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.pNext = nullptr;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = m_GraphicsQueueFamily;

		if (vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_SingleUseCommandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create command pool");
		}
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

		VkCommandBufferSubmitInfo cmdInfo = VulkanInitializers::CommandBufferSubmitInfo(cmd);
		VkSubmitInfo2 submitInfo = VulkanInitializers::SubmitInfo(&cmdInfo, nullptr, nullptr);

		vkQueueSubmit2(m_GraphicsQueue, 1, &submitInfo, m_ImmFence);
		vkWaitForFences(m_Device, 1, &m_ImmFence, true, 9999999999);
	}

}