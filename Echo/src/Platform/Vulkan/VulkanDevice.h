#pragma once

#include "Core/Window.h"
#include "Graphics/Device.h"
#include "Graphics/Framebuffer.h"

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "Utils/VulkanTypes.h"
#include "Utils/VulkanResourceManager.h"
#include "Shader/ShaderCompiler.h"
#include "Windows/WindowsWindow.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <optional>

namespace Echo
{
    // Configuration for Vulkan initialization
    struct VulkanDeviceConfig
    {
        bool enableValidationLayers = true;
        bool enableDebugMessenger = true;
        uint32_t vulkanApiVersion = VK_API_VERSION_1_3;
        bool enableMultisampling = true;
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_4_BIT;
        
        // Features to enable
        bool enableDynamicRendering = true;
        bool enableSynchronization2 = true;
        bool enableBufferDeviceAddress = true;
        bool enableDescriptorIndexing = true;
        bool enableRuntimeDescriptorArray = true;
        bool enableShaderStorageImageMultisample = true;
        bool enableSampleRateShading = true;
        bool enableIndependentBlend = true;
        bool enableRobustBufferAccess = true;
    };

    struct LazyFrameData
    {
        VkSemaphore SwapchainSemaphore = VK_NULL_HANDLE;
        VkSemaphore RenderSemaphore = VK_NULL_HANDLE;
        VkFence RenderFence = VK_NULL_HANDLE;

        VkCommandPool CommandPool = VK_NULL_HANDLE;
        VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

        uint32_t ImageIndex = 0;
        bool IsFirstPass = true;
    };

    class VulkanSwapchain;
    class VulkanFramebuffer;
    class VulkanTexture2D;

    class VulkanDevice : public Device
    {
    public:
        VulkanDevice(Window* window, unsigned int width, unsigned int height, const VulkanDeviceConfig& config = VulkanDeviceConfig{});
        virtual ~VulkanDevice();

        // Device interface implementation
        virtual const DeviceType GetDeviceType() const override { return DeviceType::Vulkan; }
        virtual const DeviceState GetState() const override { return m_State.load(); }
        virtual const uint32_t GetMaxTextureSlots() const override;

        // Lazy initialization interface
        virtual bool IsInitialized() const override { return m_State.load() == DeviceState::Ready; }
        virtual bool Initialize() override;
        virtual void Shutdown() override;
        virtual void SetStateChangeCallback(StateChangeCallback callback) override { m_StateChangeCallback = callback; }
        virtual bool EnsureInitialized() override;

        // Vulkan-specific getters (require initialization)
        LazyFrameData& GetFrameData();
        ShaderLibrary GetShaderLibrary();
        VkInstance GetInstance();
        VkQueue GetGraphicsQueue();
        VkQueue GetPresentQueue();
        VkDevice GetDevice();
        VkPhysicalDevice GetPhysicalDevice();
        VkSurfaceKHR GetSurface();
        VkDescriptorPool GetImGuiDescriptorPool();
        uint32_t GetGraphicsQueueFamily();
        uint32_t GetPresentQueueFamily();
        VulkanSwapchain& GetSwapchain();
        VmaAllocator GetAllocator();
        VkExtent2D GetDrawExtent();

        // Resource management methods (all call EnsureInitialized internally)
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

        // Framebuffer management
        void AddFramebuffer(VulkanFramebuffer* framebuffer);
        void AddImGuiFramebuffer(VulkanFramebuffer* framebuffer);
        std::vector<VulkanFramebuffer*> GetImGuiFramebuffers();
        void AddImGuiTexture(VulkanTexture2D* texture);
        std::vector<VulkanTexture2D*> GetImGuiTextures();
        void AddFrame();

    private:
        // Initialization stages (used by lazy init)
        bool InitializeVulkanCore();
        bool InitializeSwapchain();
        bool InitializeSyncStructures();
        bool InitializeCommands();
        bool CreateImGuiDescriptorPool();
        void SetState(DeviceState newState);
        bool CheckInitialized() const;

        // Core Vulkan objects (using RAII wrappers)
        VulkanInstance m_Instance;
        VulkanManagedDevice m_VkDevice;
        VulkanSurface m_Surface;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;
        uint32_t m_GraphicsQueueFamily = 0;
        uint32_t m_PresentQueueFamily = 0;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
        VkFence m_ImmFence = VK_NULL_HANDLE;
        VkCommandBuffer m_ImmCommandBuffer = VK_NULL_HANDLE;
        VkCommandPool m_ImmCommandPool = VK_NULL_HANDLE;
        VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;
        VmaAllocator m_Allocator = VK_NULL_HANDLE;
        VkExtent2D m_DrawExtent{};
        LazyFrameData m_Frames[MAX_FRAMES_IN_FLIGHT];
        uint32_t m_CurrentFrame = 0;
        std::optional<ShaderLibrary> m_ShaderLibrary;
        Scope<VulkanSwapchain> m_Swapchain;
        std::vector<VulkanFramebuffer*> m_Framebuffers;
        std::vector<VulkanFramebuffer*> m_ImGuiFramebuffers;
        std::vector<VulkanTexture2D*> m_ImGuiTextures;
        Window* m_Window;
        HWND m_WindowHandle;
        unsigned int m_Width;
        unsigned int m_Height;
        VulkanDeviceConfig m_Config;
        mutable std::mutex m_InitMutex;
        std::atomic<DeviceState> m_State{DeviceState::Uninitialized};
        StateChangeCallback m_StateChangeCallback;
        std::string m_LastError;
    };
} 