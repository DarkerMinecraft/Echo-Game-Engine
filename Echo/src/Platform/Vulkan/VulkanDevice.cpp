#include "pch.h"
#include "VulkanDevice.h"

#include "VulkanSwapchain.h"
#include "VulkanCommandList.h"
#include "VulkanSynchronization.h"
#include "Utils/VulkanImages.h"
#include "Utils/VulkanInitializers.h"

#include "Echo/Core/Application.h"

#include <VkBootstrap.h>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <backends/imgui_impl_vulkan.h>

namespace Echo 
{

	VulkanDevice::VulkanDevice(void* window)
		: m_Window((GLFWwindow*) window)
	{
		InitVulkan();
		InitSwapchain();
		InitCommands();
		InitSyncStructures();
		InitDescriptors();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDeviceWaitIdle(m_Device);

		for (int i = 0; i < FRAME_OVERLAP; i++) 
		{
			m_Frames[i].Pool.reset();
			
			m_Frames[i].RenderFence.reset();
			m_Frames[i].RenderSemaphore.reset();
			m_Frames[i].SwapchainSemaphore.reset();
		}

		m_DeletionQueue.Flush();

		m_Swapchain.reset();
		
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		vkb::destroy_debug_utils_messenger(m_Instance, m_DebugMessenger);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanDevice::SetClearColor(const glm::vec4& color)
	{
		m_ClearColorValue = { {color.r, color.g, color.b, color.a} };
	}

	void VulkanDevice::DrawBackground(Ref<Pipeline> pipeline)
	{
		pipeline->Bind();
		vkCmdDispatch(m_CurrentBuffer, std::ceil(m_DrawExtent.width / 16.0), std::ceil(m_DrawExtent.height / 16.0), 1);
	}

	void VulkanDevice::Start()
	{
		m_DrawExtent.width = m_AllocatedImage.ImageExtent.width;
		m_DrawExtent.height = m_AllocatedImage.ImageExtent.height;

		GetCurrentFrame().RenderFence->Wait();
		GetCurrentFrame().DeletionQueue.Flush();
		GetCurrentFrame().RenderFence->Reset();

		m_ImageIndex = m_Swapchain->AcquireNextImage();
		GetCurrentFrame().Buffer->Begin();
		m_CurrentBuffer = (VkCommandBuffer) GetCurrentFrame().Buffer->GetBuffer();

		m_CurrentImage = (VkImage) m_Swapchain->GetSwapchainImage(m_ImageIndex);

		VulkanImages::TransitionImage(m_CurrentBuffer, m_AllocatedImage.Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanDevice::End()
	{
		VkExtent2D swapchainExtent = {
			m_Swapchain->GetExtent().Width,
			m_Swapchain->GetExtent().Height,
		};

		VulkanImages::TransitionImage(m_CurrentBuffer, m_CurrentImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		VulkanImages::TransitionImage(m_CurrentBuffer, m_AllocatedImage.Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		VulkanImages::CopyImageToImage(m_CurrentBuffer, m_AllocatedImage.Image, m_CurrentImage, m_DrawExtent, swapchainExtent);
		VulkanImages::TransitionImage(m_CurrentBuffer, m_CurrentImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		Application::Get().GetImGuiLayer()->DrawImGui();

		VulkanImages::TransitionImage(m_CurrentBuffer, m_CurrentImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
		GetCurrentFrame().Buffer->End();

		VkCommandBufferSubmitInfo cmdInfo = VulkanInitializers::CommandBufferSubmitInfo(m_CurrentBuffer);
		VkSemaphore renderWaitSemaphore = (VkSemaphore)GetCurrentFrame().SwapchainSemaphore->GetSemaphore();
		VkSemaphoreSubmitInfo renderWaitInfo = VulkanInitializers::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, renderWaitSemaphore);
		VkSemaphore renderSignalSemaphore = (VkSemaphore)GetCurrentFrame().RenderSemaphore->GetSemaphore();
		VkSemaphoreSubmitInfo renderSignalInfo = VulkanInitializers::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, renderSignalSemaphore);

		VkSubmitInfo2 submit = VulkanInitializers::SubmitInfo(&cmdInfo, &renderSignalInfo, &renderWaitInfo);
		VkFence renderFence = (VkFence)GetCurrentFrame().RenderFence->GetFence();

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

		if (presentResult != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swapchain image!");
		}

		m_FrameNumber++;
	}



	void VulkanDevice::InitVulkan()
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

		glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);

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
			.add_required_extension("VK_KHR_dynamic_rendering")
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

		m_DeletionQueue.PushFunction([&]()
		{
			vmaDestroyAllocator(m_Allocator);
		});
	}

	void VulkanDevice::InitSwapchain()
	{
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);

		m_Swapchain = CreateScope<VulkanSwapchain>(this, static_cast<uint32_t>(width), static_cast<uint32_t>(height)); 

		VkExtent3D drawImageExtent =
		{
			width,
			height,
			1
		};

		m_AllocatedImage.ImageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
		m_AllocatedImage.ImageExtent = drawImageExtent;

		VkImageUsageFlags drawImageUsages{};
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
		drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		VkImageCreateInfo rimg_info = VulkanInitializers::ImageCreateInfo(m_AllocatedImage.ImageFormat, drawImageUsages, drawImageExtent);

		VmaAllocationCreateInfo rimg_allocinfo = {};
		rimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		rimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vmaCreateImage(m_Allocator, &rimg_info, &rimg_allocinfo, &m_AllocatedImage.Image, &m_AllocatedImage.Allocation, nullptr);

		VkImageViewCreateInfo rview_info = VulkanInitializers::ImageViewCreateInfo(m_AllocatedImage.ImageFormat, m_AllocatedImage.Image, VK_IMAGE_ASPECT_COLOR_BIT);

		if (vkCreateImageView(m_Device, &rview_info, nullptr, &m_AllocatedImage.ImageView) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create image view");
		}

		m_DeletionQueue.PushFunction([=]()
		{
			vkDestroyImageView(m_Device, m_AllocatedImage.ImageView, nullptr);
			vmaDestroyImage(m_Allocator, m_AllocatedImage.Image, m_AllocatedImage.Allocation);
		});
	}

	void VulkanDevice::InitCommands()
	{
		for (int i = 0; i < FRAME_OVERLAP; i++) 
		{
			m_Frames[i].Pool = CreateScope<VulkanCommandPool>(this, m_GraphicsQueueFamily, FRAME_OVERLAP);
			m_Frames[i].Buffer = CreateScope<VulkanCommandBuffer>(this, (VkCommandPool)m_Frames[i].Pool->GetPool());
		}
	}

	void VulkanDevice::InitSyncStructures()
	{
		for (int i = 0; i < FRAME_OVERLAP; i++) 
		{
			m_Frames[i].RenderFence = CreateScope<VulkanFence>(this);

			m_Frames[i].SwapchainSemaphore = CreateScope<VulkanSemaphore>(this);
			m_Frames[i].RenderSemaphore = CreateScope<VulkanSemaphore>(this);
		}
	}

	void VulkanDevice::InitDescriptors()
	{
		std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
		{
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
		};

		m_DescriptorAllocator.InitPool(m_Device, 10, sizes);

		{
			DescriptorLayoutBuilder builder;
			builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			m_DrawImageDescriptorLayout = builder.Build(m_Device, VK_SHADER_STAGE_COMPUTE_BIT);
		}

		m_DrawImageDescriptors = m_DescriptorAllocator.Allocate(m_Device, m_DrawImageDescriptorLayout);

		VkDescriptorImageInfo imgInfo{};
		imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imgInfo.imageView = m_AllocatedImage.ImageView;

		VkWriteDescriptorSet drawImageWrite = {};
		drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		drawImageWrite.pNext = nullptr;

		drawImageWrite.dstBinding = 0;
		drawImageWrite.dstSet = m_DrawImageDescriptors;
		drawImageWrite.descriptorCount = 1;
		drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		drawImageWrite.pImageInfo = &imgInfo;

		vkUpdateDescriptorSets(m_Device, 1, &drawImageWrite, 0, nullptr);

		m_DeletionQueue.PushFunction([&]()
		{
			m_DescriptorAllocator.DestroyPool(m_Device);

			vkDestroyDescriptorSetLayout(m_Device, m_DrawImageDescriptorLayout, nullptr);
		});
	}

}