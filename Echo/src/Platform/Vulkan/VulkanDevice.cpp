#include "pch.h"
#include "VulkanDevice.h"
#include "VulkanRenderCaps.h"

#include "Utils/VulkanInitializers.h"
#include "Utils/VulkanImages.h"

#include "VkBootstrap.h"
#include "VulkanSwapchain.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "VulkanCommandBuffer.h"
#include "VulkanFramebuffer.h"
#include "Utils/DeferredInitManager.h"

#include <glm/glm.hpp>
#include <array>

namespace Echo 
{
    VulkanDevice::VulkanDevice(Window* window, unsigned int width, unsigned int height, 
                                     const VulkanDeviceConfig& config)
        : m_Window(window), m_WindowHandle((HWND)window->GetNativeWindow()), 
          m_Width(width), m_Height(height), m_Config(config)
    {
        EC_PROFILE_FUNCTION();
        EC_CORE_INFO("VulkanDevice created - initialization deferred");
        
        // Only store configuration, don't initialize anything yet
        SetState(DeviceState::Uninitialized);
    }

    VulkanDevice::~VulkanDevice()
    {
        EC_PROFILE_FUNCTION();
        Shutdown();
    }

    bool VulkanDevice::Initialize()
    {
        EC_PROFILE_FUNCTION();
        std::lock_guard<std::mutex> lock(m_InitMutex);
        
        if (m_State.load() == DeviceState::Ready)
        {
            return true;
        }
        
        if (m_State.load() == DeviceState::Initializing)
        {
            EC_CORE_WARN("Vulkan device is already initializing");
            return false;
        }

        SetState(DeviceState::Initializing);
        EC_CORE_INFO("Starting Vulkan device initialization...");

        // Initialize in stages for better error handling and modularity
        if (!InitializeVulkanCore())
        {
            SetState(DeviceState::Failed);
            return false;
        }

        if (!InitializeSyncStructures())
        {
            SetState(DeviceState::Failed);
            return false;
        }

        if (!InitializeCommands())
        {
            SetState(DeviceState::Failed);
            return false;
        }

        if (!InitializeSwapchain())
        {
            SetState(DeviceState::Failed);
            return false;
        }

        if (!CreateImGuiDescriptorPool())
        {
            SetState(DeviceState::Failed);
            return false;
        }

        // Initialize shader library
        m_ShaderLibrary = ShaderLibrary(m_VkDevice.Get());

        VulkanRenderCaps::Init(this);

        SetState(DeviceState::Ready);
		// Process all deferred initialization tasks now that the device is ready
		EC_CORE_INFO("[VulkanDevice] Processing deferred initialization tasks at end of Initialize");
		Echo::DeferredInitManager::Process([] { return true; });

        EC_CORE_INFO("Vulkan device initialization completed successfully");
        return true;
    }

    bool VulkanDevice::EnsureInitialized()
    {
        if (IsInitialized())
            return true;
        return Initialize();
    }

    void VulkanDevice::Shutdown()
    {
        EC_PROFILE_FUNCTION();
        std::lock_guard<std::mutex> lock(m_InitMutex);
        
        if (m_State.load() == DeviceState::Uninitialized)
        {
            return;
        }

        if (m_VkDevice.IsValid())
        {
            vkDeviceWaitIdle(m_VkDevice.Get());
        }

        EC_CORE_INFO("Shutting down Vulkan device...");

        // Cleanup in reverse order of initialization
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (m_Frames[i].CommandPool != VK_NULL_HANDLE)
            {
                vkDestroyCommandPool(m_VkDevice.Get(), m_Frames[i].CommandPool, nullptr);
                m_Frames[i].CommandPool = VK_NULL_HANDLE;
            }

            if (m_Frames[i].SwapchainSemaphore != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(m_VkDevice.Get(), m_Frames[i].SwapchainSemaphore, nullptr);
                m_Frames[i].SwapchainSemaphore = VK_NULL_HANDLE;
            }

            if (m_Frames[i].RenderSemaphore != VK_NULL_HANDLE)
            {
                vkDestroySemaphore(m_VkDevice.Get(), m_Frames[i].RenderSemaphore, nullptr);
                m_Frames[i].RenderSemaphore = VK_NULL_HANDLE;
            }

            if (m_Frames[i].RenderFence != VK_NULL_HANDLE)
            {
                vkDestroyFence(m_VkDevice.Get(), m_Frames[i].RenderFence, nullptr);
                m_Frames[i].RenderFence = VK_NULL_HANDLE;
            }
        }

        if (m_ImmCommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_VkDevice.Get(), m_ImmCommandPool, nullptr);
            m_ImmCommandPool = VK_NULL_HANDLE;
        }

        if (m_ImmFence != VK_NULL_HANDLE)
        {
            vkDestroyFence(m_VkDevice.Get(), m_ImmFence, nullptr);
            m_ImmFence = VK_NULL_HANDLE;
        }

        if (m_ImGuiDescriptorPool != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorPool(m_VkDevice.Get(), m_ImGuiDescriptorPool, nullptr);
            m_ImGuiDescriptorPool = VK_NULL_HANDLE;
        }

        if (m_Allocator != VK_NULL_HANDLE)
        {
            vmaDestroyAllocator(m_Allocator);
            m_Allocator = VK_NULL_HANDLE;
        }

        m_Swapchain.reset();

        if (m_DebugMessenger != VK_NULL_HANDLE)
        {
            vkb::destroy_debug_utils_messenger(m_Instance.Get(), m_DebugMessenger);
            m_DebugMessenger = VK_NULL_HANDLE;
        }

        // Reset RAII objects (they will automatically clean up)
        m_VkDevice.Reset();
        m_Surface.Reset();
        m_Instance.Reset();

        SetState(DeviceState::Uninitialized);
        EC_CORE_INFO("Vulkan device shutdown completed");
    }

    bool VulkanDevice::InitializeVulkanCore()
    {
        EC_PROFILE_FUNCTION();
        
        try 
        {
            // Create Vulkan instance
            vkb::InstanceBuilder builder;
            auto inst_ret = builder.set_app_name("Echo Engine")
                .request_validation_layers(m_Config.enableValidationLayers)
                .use_default_debug_messenger()
                .require_api_version(1, 4, 304)
                .build();

            if (!inst_ret.has_value())
            {
                m_LastError = "Failed to create Vulkan instance: " + inst_ret.error().message();
                EC_CORE_ERROR(m_LastError);
                return false;
            }

            vkb::Instance vkb_inst = inst_ret.value();
            m_Instance = VulkanResourceFactory::CreateInstance(vkb_inst.instance, "MainInstance");
            m_DebugMessenger = vkb_inst.debug_messenger;

            // Create surface
            VkSurfaceKHR surface = m_Window->SetWindowSurface(m_Instance.Get());
            m_Surface = VulkanResourceFactory::CreateSurface(surface, m_Instance.Get(), "MainSurface");

            // Setup device features
            VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
            features.dynamicRendering = m_Config.enableDynamicRendering;
            features.synchronization2 = m_Config.enableSynchronization2;
            
            VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
            features12.bufferDeviceAddress = m_Config.enableBufferDeviceAddress;
            features12.descriptorIndexing = m_Config.enableDescriptorIndexing;
            features12.runtimeDescriptorArray = m_Config.enableRuntimeDescriptorArray;

            VkPhysicalDeviceFeatures deviceFeatures{};
            deviceFeatures.shaderStorageImageMultisample = m_Config.enableShaderStorageImageMultisample;
            deviceFeatures.sampleRateShading = m_Config.enableSampleRateShading;
            deviceFeatures.independentBlend = m_Config.enableIndependentBlend;
            deviceFeatures.robustBufferAccess = m_Config.enableRobustBufferAccess;

            // Select physical device
            vkb::PhysicalDeviceSelector selector{ vkb_inst };
            auto physicalDevice_ret = selector
                .set_minimum_version(1, 3)
                .set_required_features_13(features)
                .set_required_features_12(features12)
                .set_required_features(deviceFeatures)
                .set_surface(m_Surface.Get())
                .select();

            if (!physicalDevice_ret.has_value())
            {
                m_LastError = "Failed to select suitable physical device: " + physicalDevice_ret.error().message();
                EC_CORE_ERROR(m_LastError);
                return false;
            }

            vkb::PhysicalDevice physicalDevice = physicalDevice_ret.value();
            m_PhysicalDevice = physicalDevice.physical_device;

            // Create logical device
            vkb::DeviceBuilder deviceBuilder{ physicalDevice };
            auto vkbDevice_ret = deviceBuilder.build();
            
            if (!vkbDevice_ret.has_value())
            {
                m_LastError = "Failed to create logical device: " + vkbDevice_ret.error().message();
                EC_CORE_ERROR(m_LastError);
                return false;
            }

            vkb::Device vkbDevice = vkbDevice_ret.value();
            m_VkDevice = VulkanResourceFactory::CreateDevice(vkbDevice.device, "MainDevice");

            // Get queues
            auto graphics_queue_ret = vkbDevice.get_queue(vkb::QueueType::graphics);
            auto present_queue_ret = vkbDevice.get_queue(vkb::QueueType::present);
            
            if (!graphics_queue_ret.has_value() || !present_queue_ret.has_value())
            {
                m_LastError = "Failed to get device queues";
                EC_CORE_ERROR(m_LastError);
                return false;
            }

            m_GraphicsQueue = graphics_queue_ret.value();
            m_GraphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
            m_PresentQueue = present_queue_ret.value();
            m_PresentQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::present).value();

            // Create VMA allocator
            VmaAllocatorCreateInfo allocatorInfo = {};
            allocatorInfo.physicalDevice = m_PhysicalDevice;
            allocatorInfo.device = m_VkDevice.Get();
            allocatorInfo.instance = m_Instance.Get();
            allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
            
            VkResult result = vmaCreateAllocator(&allocatorInfo, &m_Allocator);
            if (result != VK_SUCCESS)
            {
                m_LastError = "Failed to create VMA allocator";
                EC_CORE_ERROR(m_LastError);
                return false;
            }

            EC_CORE_INFO("Vulkan core initialization completed");
            return true;
        }
        catch (const std::exception& e)
        {
            m_LastError = "Exception during Vulkan core initialization: " + std::string(e.what());
            EC_CORE_ERROR(m_LastError);
            return false;
        }
    }

    bool VulkanDevice::InitializeSwapchain()
    {
        EC_PROFILE_FUNCTION();
        
        try 
        {
            // TODO: VulkanSwapchain needs to be refactored to work with VulkanDevice
            // For now, we'll skip swapchain creation to get the build working
            // m_Swapchain = CreateScope<VulkanSwapchain>(this, m_Width, m_Height);
            m_DrawExtent = {
                static_cast<uint32_t>(m_Width),
                static_cast<uint32_t>(m_Height)
            };
            
            EC_CORE_INFO("Swapchain initialization completed (placeholder)");
            return true;
        }
        catch (const std::exception& e)
        {
            m_LastError = "Exception during swapchain initialization: " + std::string(e.what());
            EC_CORE_ERROR(m_LastError);
            return false;
        }
    }

    bool VulkanDevice::InitializeSyncStructures()
    {
        EC_PROFILE_FUNCTION();
        
        try 
        {
            VkSemaphoreCreateInfo semaphoreCreateInfo = VulkanInitializers::SemaphoreCreateInfo();
            VkFenceCreateInfo fenceCreateInfo = VulkanInitializers::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
            {
                VkResult result;
                
                result = vkCreateSemaphore(m_VkDevice.Get(), &semaphoreCreateInfo, nullptr, &m_Frames[i].RenderSemaphore);
                if (result != VK_SUCCESS)
                {
                    m_LastError = "Failed to create render semaphore for frame " + std::to_string(i);
                    EC_CORE_ERROR(m_LastError);
                    return false;
                }

                result = vkCreateSemaphore(m_VkDevice.Get(), &semaphoreCreateInfo, nullptr, &m_Frames[i].SwapchainSemaphore);
                if (result != VK_SUCCESS)
                {
                    m_LastError = "Failed to create swapchain semaphore for frame " + std::to_string(i);
                    EC_CORE_ERROR(m_LastError);
                    return false;
                }

                result = vkCreateFence(m_VkDevice.Get(), &fenceCreateInfo, nullptr, &m_Frames[i].RenderFence);
                if (result != VK_SUCCESS)
                {
                    m_LastError = "Failed to create render fence for frame " + std::to_string(i);
                    EC_CORE_ERROR(m_LastError);
                    return false;
                }
            }

            VkResult result = vkCreateFence(m_VkDevice.Get(), &fenceCreateInfo, nullptr, &m_ImmFence);
            if (result != VK_SUCCESS)
            {
                m_LastError = "Failed to create immediate fence";
                EC_CORE_ERROR(m_LastError);
                return false;
            }

            EC_CORE_INFO("Synchronization structures initialization completed");
            return true;
        }
        catch (const std::exception& e)
        {
            m_LastError = "Exception during sync structures initialization: " + std::string(e.what());
            EC_CORE_ERROR(m_LastError);
            return false;
        }
    }

    bool VulkanDevice::InitializeCommands()
    {
        EC_PROFILE_FUNCTION();
        
        try 
        {
            VkCommandPoolCreateInfo poolCreateInfo = VulkanInitializers::CommandPoolCreateInfo(
                VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_GraphicsQueueFamily);
            
            for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                VkResult result = vkCreateCommandPool(m_VkDevice.Get(), &poolCreateInfo, nullptr, &m_Frames[i].CommandPool);
                if (result != VK_SUCCESS)
                {
                    m_LastError = "Failed to create command pool for frame " + std::to_string(i);
                    EC_CORE_ERROR(m_LastError);
                    return false;
                }

                VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInitializers::CommandBufferAllocateInfo(m_Frames[i].CommandPool, 1);
                result = vkAllocateCommandBuffers(m_VkDevice.Get(), &cmdAllocInfo, &m_Frames[i].CommandBuffer);
                if (result != VK_SUCCESS)
                {
                    m_LastError = "Failed to allocate command buffer for frame " + std::to_string(i);
                    EC_CORE_ERROR(m_LastError);
                    return false;
                }
            }

            VkResult result = vkCreateCommandPool(m_VkDevice.Get(), &poolCreateInfo, nullptr, &m_ImmCommandPool);
            if (result != VK_SUCCESS)
            {
                m_LastError = "Failed to create immediate command pool";
                EC_CORE_ERROR(m_LastError);
                return false;
            }

            VkCommandBufferAllocateInfo cmdAllocInfo = VulkanInitializers::CommandBufferAllocateInfo(m_ImmCommandPool, 1);
            result = vkAllocateCommandBuffers(m_VkDevice.Get(), &cmdAllocInfo, &m_ImmCommandBuffer);
            if (result != VK_SUCCESS)
            {
                m_LastError = "Failed to allocate immediate command buffer";
                EC_CORE_ERROR(m_LastError);
                return false;
            }

            EC_CORE_INFO("Command structures initialization completed");
            return true;
        }
        catch (const std::exception& e)
        {
            m_LastError = "Exception during command initialization: " + std::string(e.what());
            EC_CORE_ERROR(m_LastError);
            return false;
        }
    }

    bool VulkanDevice::CreateImGuiDescriptorPool()
    {
        EC_PROFILE_FUNCTION();
        
        try 
        {
            VkDescriptorPoolSize pool_sizes[] = { 
                { VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 } 
            };

            VkDescriptorPoolCreateInfo pool_info = {};
            pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            pool_info.maxSets = 1000;
            pool_info.poolSizeCount = static_cast<uint32_t>(std::size(pool_sizes));
            pool_info.pPoolSizes = pool_sizes;

            VkResult result = vkCreateDescriptorPool(m_VkDevice.Get(), &pool_info, nullptr, &m_ImGuiDescriptorPool);
            if (result != VK_SUCCESS)
            {
                m_LastError = "Failed to create ImGui descriptor pool";
                EC_CORE_ERROR(m_LastError);
                return false;
            }

            EC_CORE_INFO("ImGui descriptor pool initialization completed");
            return true;
        }
        catch (const std::exception& e)
        {
            m_LastError = "Exception during ImGui descriptor pool creation: " + std::string(e.what());
            EC_CORE_ERROR(m_LastError);
            return false;
        }
    }

    void VulkanDevice::SetState(DeviceState newState)
    {
        DeviceState oldState = m_State.exchange(newState);
        if (oldState != newState && m_StateChangeCallback)
        {
            m_StateChangeCallback(oldState, newState);
        }
    }

    bool VulkanDevice::CheckInitialized() const
    {
        if (!IsInitialized())
        {
            EC_CORE_ERROR("Vulkan device operation called before initialization");
            return false;
        }
        return true;
    }

    // Implementation of methods that require initialization
    const uint32_t VulkanDevice::GetMaxTextureSlots() const
    {
        if (!CheckInitialized()) return 0;
        // Return a default value for now since VulkanRenderCaps needs refactoring
        return 32;
    }

    LazyFrameData& VulkanDevice::GetFrameData()
    {
        return m_Frames[m_CurrentFrame % MAX_FRAMES_IN_FLIGHT];
    }

    ShaderLibrary VulkanDevice::GetShaderLibrary()
    {
        return m_ShaderLibrary.value();
    }

    VkInstance VulkanDevice::GetInstance()
    {
        return m_Instance.Get();
    }

    VkQueue VulkanDevice::GetGraphicsQueue()
    {
        return m_GraphicsQueue;
    }

    VkQueue VulkanDevice::GetPresentQueue()
    {
        return m_PresentQueue;
    }

    VkDevice VulkanDevice::GetDevice()
    {
        return m_VkDevice.Get();
    }

    VkPhysicalDevice VulkanDevice::GetPhysicalDevice()
    {
        return m_PhysicalDevice;
    }

    VkSurfaceKHR VulkanDevice::GetSurface()
    {
        return m_Surface.Get();
    }

    VkDescriptorPool VulkanDevice::GetImGuiDescriptorPool()
    {
        return m_ImGuiDescriptorPool;
    }

    uint32_t VulkanDevice::GetGraphicsQueueFamily()
    {
        return m_GraphicsQueueFamily;
    }

    uint32_t VulkanDevice::GetPresentQueueFamily()
    {
        return m_PresentQueueFamily;
    }

    VulkanSwapchain& VulkanDevice::GetSwapchain()
    {
        return *m_Swapchain.get();  
    }

    VmaAllocator VulkanDevice::GetAllocator()
    {
        return m_Allocator;
    }

    VkExtent2D VulkanDevice::GetDrawExtent()
    {
        return m_DrawExtent;
    }

    // Resource management methods implementation (delegate to original methods)
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
        if (!IsInitialized()) return;
        EC_PROFILE_FUNCTION();
        vmaDestroyBuffer(m_Allocator, buffer.Buffer, buffer.Allocation);
    }

    void VulkanDevice::AddFramebuffer(VulkanFramebuffer* framebuffer)
    {
        m_Framebuffers.push_back(framebuffer);
    }

    void VulkanDevice::AddImGuiFramebuffer(VulkanFramebuffer* framebuffer)
    {
        m_ImGuiFramebuffers.push_back(framebuffer);
    }

    std::vector<VulkanFramebuffer*> VulkanDevice::GetImGuiFramebuffers()
    {
        return m_ImGuiFramebuffers;
    }

    void VulkanDevice::AddImGuiTexture(VulkanTexture2D* texture)
    {
        m_ImGuiTextures.push_back(texture);
    }

    std::vector<VulkanTexture2D*> VulkanDevice::GetImGuiTextures()
    {
        return m_ImGuiTextures;
    }

    void VulkanDevice::AddFrame()
    {
        m_CurrentFrame++;
    }

    void VulkanDevice::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
    {
        EC_PROFILE_FUNCTION();
        
        vkResetFences(m_VkDevice.Get(), 1, &m_ImmFence);
        vkResetCommandBuffer(m_ImmCommandBuffer, 0);

        VkCommandBuffer cmd = m_ImmCommandBuffer;
        VkCommandBufferBeginInfo cmdBeginInfo = VulkanInitializers::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        vkBeginCommandBuffer(cmd, &cmdBeginInfo);
        function(cmd);
        vkEndCommandBuffer(cmd);

        VkCommandBufferSubmitInfo cmdinfo = VulkanInitializers::CommandBufferSubmitInfo(cmd);
        VkSubmitInfo2 submit = VulkanInitializers::SubmitInfo(&cmdinfo, nullptr, nullptr);

        vkQueueSubmit2(m_GraphicsQueue, 1, &submit, m_ImmFence);
        vkWaitForFences(m_VkDevice.Get(), 1, &m_ImmFence, true, UINT64_MAX);
    }

    void* VulkanDevice::GetMappedData(const AllocatedBuffer& buffer)
    {
        if (!IsInitialized()) return nullptr;
        return buffer.Info.pMappedData;
    }

    void VulkanDevice::RecreateSwapchain(int width, int height, VulkanSwapchain* oldSwapchain)
    {
        EC_PROFILE_FUNCTION();
        
        vkDeviceWaitIdle(m_VkDevice.Get());

        m_Swapchain = CreateScope<VulkanSwapchain>(this, oldSwapchain, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        m_DrawExtent = {
            static_cast<uint32_t>(width), 
            static_cast<uint32_t>(height)
        };

        for (auto& fb : m_Framebuffers)
        {
            fb->UpdateSize();
        }
        
        EC_CORE_WARN("RecreateSwapchain not fully implemented in lazy device yet");
    }

    // Implement remaining image creation methods following the same pattern...
    AllocatedImage VulkanDevice::CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage)
    {
        AllocatedImage newImage{};
        
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent = size;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = m_Config.msaaSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VkResult result = vmaCreateImage(m_Allocator, &imageInfo, &allocInfo, &newImage.Image, &newImage.Allocation, nullptr);
        if (result != VK_SUCCESS) {
            EC_CORE_ERROR("Failed to create Vulkan image");
            return {};
        }
        newImage.ImageExtent = size;
        newImage.ImageFormat = format;
        newImage.ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        newImage.Samples = m_Config.msaaSamples;
        newImage.DepthTexture = (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
        newImage.Destroyed = false;

        // Create image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = newImage.Image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(m_VkDevice.Get(), &viewInfo, nullptr, &newImage.ImageView);
        if (result != VK_SUCCESS) {
            EC_CORE_ERROR("Failed to create Vulkan image view");
            vmaDestroyImage(m_Allocator, newImage.Image, newImage.Allocation);
            return {};
        }
        return newImage;
    }

    AllocatedImage VulkanDevice::CreateImageNoMSAA(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped)
    {
        AllocatedImage newImage{};
        
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent = size;
        imageInfo.mipLevels = mipmapped ? static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1 : 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VkResult result = vmaCreateImage(m_Allocator, &imageInfo, &allocInfo, &newImage.Image, &newImage.Allocation, nullptr);
        if (result != VK_SUCCESS) {
            EC_CORE_ERROR("Failed to create Vulkan image (no MSAA)");
            return {};
        }
        newImage.ImageExtent = size;
        newImage.ImageFormat = format;
        newImage.ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        newImage.Samples = VK_SAMPLE_COUNT_1_BIT;
        newImage.DepthTexture = (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
        newImage.Destroyed = false;

        // Create image view
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = newImage.Image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = imageInfo.mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(m_VkDevice.Get(), &viewInfo, nullptr, &newImage.ImageView);
        if (result != VK_SUCCESS) {
            EC_CORE_ERROR("Failed to create Vulkan image view (no MSAA)");
            vmaDestroyImage(m_Allocator, newImage.Image, newImage.Allocation);
            return {};
        }
        return newImage;
    }

    AllocatedImage VulkanDevice::CreateImageTex(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped)
    {
        // Create the image
        AllocatedImage image = CreateImageNoMSAA(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT, mipmapped);
        if (!image.Image) {
            EC_CORE_ERROR("Failed to create image for texture upload");
            return {};
        }
        // Create staging buffer
        size_t imageSize = size.width * size.height * 4; // Assume 4 bytes per pixel (RGBA8)
        AllocatedBuffer staging = CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
        void* mapped = GetMappedData(staging);
        memcpy(mapped, data, imageSize);
        // Copy buffer to image
        ImmediateSubmit([&](VkCommandBuffer cmd) {
            VkImageMemoryBarrier barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = image.Image;
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
            VkBufferImageCopy copyRegion{};
            copyRegion.bufferOffset = 0;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;
            copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.imageSubresource.mipLevel = 0;
            copyRegion.imageSubresource.baseArrayLayer = 0;
            copyRegion.imageSubresource.layerCount = 1;
            copyRegion.imageOffset = {0, 0, 0};
            copyRegion.imageExtent = size;
            vkCmdCopyBufferToImage(cmd, staging.Buffer, image.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
            // Transition to shader read layout
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        });
        DestroyBuffer(staging);
        return image;
    }

    void VulkanDevice::DestroyImage(const AllocatedImage& image)
    {
        if (!IsInitialized()) return;
        EC_PROFILE_FUNCTION();
        vkDestroyImageView(m_VkDevice.Get(), image.ImageView, nullptr);
        vmaDestroyImage(m_Allocator, image.Image, image.Allocation);
    }
} 