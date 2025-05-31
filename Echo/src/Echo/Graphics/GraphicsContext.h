#pragma once

#include "Device.h"
#include "Core/Base.h"
#include <functional>
#include <memory>
#include <chrono>

namespace Echo
{
    // Performance monitoring for lazy loading
    struct GraphicsContextStats
    {
        std::chrono::steady_clock::time_point initStartTime;
        std::chrono::steady_clock::time_point initEndTime;
        bool isInitialized = false;
        size_t resourcesCreated = 0;
        size_t resourcesDestroyed = 0;
        
        double GetInitializationTimeMs() const
        {
            if (initStartTime == std::chrono::steady_clock::time_point{} || 
                initEndTime == std::chrono::steady_clock::time_point{})
                return 0.0;
            
            return std::chrono::duration<double, std::milli>(initEndTime - initStartTime).count();
        }
    };

    enum class GraphicsAPI
    {
        None = 0,
        Vulkan,
        DirectX12,
        OpenGL
    };

    // High-level graphics context with lazy loading capabilities
    class GraphicsContext
    {
    public:
        using InitializationCallback = std::function<void(bool success, const std::string& errorMessage)>;
        using ResourceCallback = std::function<void(const std::string& resourceName, bool created)>;

        explicit GraphicsContext(GraphicsAPI api = GraphicsAPI::Vulkan);
        virtual ~GraphicsContext();

        // Initialization control
        bool Initialize(Window* window, unsigned int width, unsigned int height);
        bool IsInitialized() const;
        bool EnsureInitialized();
        void Shutdown();

        // Device access (triggers lazy initialization)
        Device* GetDevice();
        const Device* GetDevice() const;

        // Configuration
        void SetPreferredAPI(GraphicsAPI api) { m_PreferredAPI = api; }
        GraphicsAPI GetCurrentAPI() const { return m_CurrentAPI; }

        // Callbacks
        void SetInitializationCallback(InitializationCallback callback) { m_InitCallback = callback; }
        void SetResourceCallback(ResourceCallback callback) { m_ResourceCallback = callback; }

        // Performance monitoring
        const GraphicsContextStats& GetStats() const { return m_Stats; }
        bool IsLazyLoadingEnabled() const { return m_LazyLoadingEnabled; }
        void SetLazyLoadingEnabled(bool enabled) { m_LazyLoadingEnabled = enabled; }

        // Resource management helpers
        template<typename ResourceType, typename... Args>
        Scope<ResourceType> CreateResource(const std::string& name, Args&&... args)
        {
            EnsureInitialized();
            
            auto resource = CreateScope<ResourceType>(std::forward<Args>(args)...);
            if (resource && m_ResourceCallback)
            {
                m_ResourceCallback(name, true);
                m_Stats.resourcesCreated++;
            }
            return resource;
        }

        void DestroyResource(const std::string& name)
        {
            if (m_ResourceCallback)
            {
                m_ResourceCallback(name, false);
                m_Stats.resourcesDestroyed++;
            }
        }

        // Utility methods
        bool SupportsAPI(GraphicsAPI api) const;
        std::vector<GraphicsAPI> GetAvailableAPIs() const;
        std::string GetAPIName(GraphicsAPI api) const;

    private:
        bool InitializeDevice();
        DeviceType ConvertToDeviceType(GraphicsAPI api) const;

        GraphicsAPI m_PreferredAPI = GraphicsAPI::Vulkan;
        GraphicsAPI m_CurrentAPI = GraphicsAPI::None;
        
        Scope<Device> m_Device;
        Window* m_Window = nullptr;
        unsigned int m_Width = 0;
        unsigned int m_Height = 0;
        
        bool m_LazyLoadingEnabled = true;
        bool m_InitializationAttempted = false;
        
        InitializationCallback m_InitCallback;
        ResourceCallback m_ResourceCallback;
        
        GraphicsContextStats m_Stats;
        std::string m_LastError;

        mutable std::mutex m_ContextMutex;
    };

    // Global graphics context accessor (singleton pattern)
    class GraphicsManager
    {
    public:
        static GraphicsManager& GetInstance()
        {
            static GraphicsManager instance;
            return instance;
        }

        bool Initialize(Window* window, unsigned int width, unsigned int height, GraphicsAPI api = GraphicsAPI::Vulkan)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (m_Context)
            {
                EC_CORE_WARN("Graphics manager already initialized");
                return m_Context->IsInitialized();
            }

            m_Context = CreateScope<GraphicsContext>(api);
            return m_Context->Initialize(window, width, height);
        }

        GraphicsContext* GetContext()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            return m_Context.get();
        }

        Device* GetDevice()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            return m_Context ? m_Context->GetDevice() : nullptr;
        }

        void Shutdown()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (m_Context)
            {
                m_Context->Shutdown();
                m_Context.reset();
            }
        }

        bool IsReady() const
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            return m_Context && m_Context->IsInitialized();
        }

    private:
        GraphicsManager() = default;
        ~GraphicsManager() { Shutdown(); }

        Scope<GraphicsContext> m_Context;
        mutable std::mutex m_Mutex;

        // Delete copy constructor and assignment operator
        GraphicsManager(const GraphicsManager&) = delete;
        GraphicsManager& operator=(const GraphicsManager&) = delete;
    };
} 