#pragma once

#include <vulkan/vulkan.h>
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>

namespace Echo
{
    // RAII wrapper for Vulkan resources
    template<typename VkHandle>
    class VulkanResource
    {
    public:
        using DestroyFunction = std::function<void(VkHandle)>;

        VulkanResource() = default;
        
        VulkanResource(VkHandle handle, DestroyFunction destroyer, const std::string& name = "")
            : m_Handle(handle), m_Destroyer(std::move(destroyer)), m_Name(name)
        {
        }

        ~VulkanResource()
        {
            if (m_Handle != VK_NULL_HANDLE && m_Destroyer)
            {
                m_Destroyer(m_Handle);
                m_Handle = VK_NULL_HANDLE;
            }
        }

        // Move constructor
        VulkanResource(VulkanResource&& other) noexcept
            : m_Handle(other.m_Handle), m_Destroyer(std::move(other.m_Destroyer)), m_Name(std::move(other.m_Name))
        {
            other.m_Handle = VK_NULL_HANDLE;
        }

        // Move assignment
        VulkanResource& operator=(VulkanResource&& other) noexcept
        {
            if (this != &other)
            {
                Reset();
                m_Handle = other.m_Handle;
                m_Destroyer = std::move(other.m_Destroyer);
                m_Name = std::move(other.m_Name);
                other.m_Handle = VK_NULL_HANDLE;
            }
            return *this;
        }

        // Delete copy constructor and assignment
        VulkanResource(const VulkanResource&) = delete;
        VulkanResource& operator=(const VulkanResource&) = delete;

        VkHandle Get() const { return m_Handle; }
        operator VkHandle() const { return m_Handle; }
        
        bool IsValid() const { return m_Handle != VK_NULL_HANDLE; }
        const std::string& GetName() const { return m_Name; }

        void Reset()
        {
            if (m_Handle != VK_NULL_HANDLE && m_Destroyer)
            {
                m_Destroyer(m_Handle);
                m_Handle = VK_NULL_HANDLE;
            }
        }

        VkHandle Release()
        {
            VkHandle handle = m_Handle;
            m_Handle = VK_NULL_HANDLE;
            return handle;
        }

    private:
        VkHandle m_Handle = VK_NULL_HANDLE;
        DestroyFunction m_Destroyer;
        std::string m_Name;
    };

    // Resource manager for tracking Vulkan resource lifetimes
    class VulkanResourceManager
    {
    public:
        static VulkanResourceManager& GetInstance()
        {
            static VulkanResourceManager instance;
            return instance;
        }

        template<typename VkHandle>
        void TrackResource(const std::string& name, VkHandle handle)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_TrackedResources[name] = reinterpret_cast<void*>(handle);
        }

        void UntrackResource(const std::string& name)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_TrackedResources.erase(name);
        }

        size_t GetTrackedResourceCount() const
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            return m_TrackedResources.size();
        }

        std::vector<std::string> GetTrackedResourceNames() const
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            std::vector<std::string> names;
            names.reserve(m_TrackedResources.size());
            for (const auto& pair : m_TrackedResources)
            {
                names.push_back(pair.first);
            }
            return names;
        }

    private:
        mutable std::mutex m_Mutex;
        std::unordered_map<std::string, void*> m_TrackedResources;
    };

    // Type aliases for common Vulkan resources
    using VulkanInstance = VulkanResource<VkInstance>;
    using VulkanManagedDevice = VulkanResource<VkDevice>;
    using VulkanSurface = VulkanResource<VkSurfaceKHR>;
    using VulkanCommandPool = VulkanResource<VkCommandPool>;
    using VulkanDescriptorPool = VulkanResource<VkDescriptorPool>;
    using VulkanSemaphore = VulkanResource<VkSemaphore>;
    using VulkanFence = VulkanResource<VkFence>;
    using VulkanImageView = VulkanResource<VkImageView>;

    // Factory functions for creating managed resources
    class VulkanResourceFactory
    {
    public:
        static VulkanInstance CreateInstance(VkInstance instance, const std::string& name = "VkInstance")
        {
            return VulkanInstance(instance, [](VkInstance inst) { vkDestroyInstance(inst, nullptr); }, name);
        }

        static VulkanManagedDevice CreateDevice(VkDevice device, const std::string& name = "VkDevice")
        {
            return VulkanManagedDevice(device, [](VkDevice dev) { vkDestroyDevice(dev, nullptr); }, name);
        }

        static VulkanSurface CreateSurface(VkSurfaceKHR surface, VkInstance instance, const std::string& name = "VkSurface")
        {
            return VulkanSurface(surface, [instance](VkSurfaceKHR surf) { vkDestroySurfaceKHR(instance, surf, nullptr); }, name);
        }

        static VulkanCommandPool CreateCommandPool(VkCommandPool pool, VkDevice device, const std::string& name = "VkCommandPool")
        {
            return VulkanCommandPool(pool, [device](VkCommandPool p) { vkDestroyCommandPool(device, p, nullptr); }, name);
        }

        static VulkanDescriptorPool CreateDescriptorPool(VkDescriptorPool pool, VkDevice device, const std::string& name = "VkDescriptorPool")
        {
            return VulkanDescriptorPool(pool, [device](VkDescriptorPool p) { vkDestroyDescriptorPool(device, p, nullptr); }, name);
        }

        static VulkanSemaphore CreateSemaphore(VkSemaphore semaphore, VkDevice device, const std::string& name = "VkSemaphore")
        {
            return VulkanSemaphore(semaphore, [device](VkSemaphore s) { vkDestroySemaphore(device, s, nullptr); }, name);
        }

        static VulkanFence CreateFence(VkFence fence, VkDevice device, const std::string& name = "VkFence")
        {
            return VulkanFence(fence, [device](VkFence f) { vkDestroyFence(device, f, nullptr); }, name);
        }

        static VulkanImageView CreateImageView(VkImageView imageView, VkDevice device, const std::string& name = "VkImageView")
        {
            return VulkanImageView(imageView, [device](VkImageView iv) { vkDestroyImageView(device, iv, nullptr); }, name);
        }
    };
} 