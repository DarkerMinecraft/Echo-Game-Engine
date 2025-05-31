#include "pch.h"
#include "GraphicsContext.h"
#include "Core/Log.h"

namespace Echo
{
    GraphicsContext::GraphicsContext(GraphicsAPI api)
        : m_PreferredAPI(api), m_CurrentAPI(GraphicsAPI::None)
    {
        EC_CORE_INFO("GraphicsContext created with preferred API: {}", GetAPIName(api));
    }

    GraphicsContext::~GraphicsContext()
    {
        Shutdown();
    }

    bool GraphicsContext::Initialize(Window* window, unsigned int width, unsigned int height)
    {
        std::lock_guard<std::mutex> lock(m_ContextMutex);
        
        if (m_InitializationAttempted)
        {
            return m_Device && m_Device->IsInitialized();
        }

        m_InitializationAttempted = true;
        m_Window = window;
        m_Width = width;
        m_Height = height;

        EC_CORE_INFO("Initializing GraphicsContext...");

        if (m_LazyLoadingEnabled)
        {
            EC_CORE_INFO("Lazy loading enabled - graphics device will initialize when first needed");
            return true; // Defer actual initialization
        }
        else
        {
            return InitializeDevice();
        }
    }

    bool GraphicsContext::InitializeDevice()
    {
        if (!m_Window)
        {
            m_LastError = "No window provided for graphics context initialization";
            EC_CORE_ERROR(m_LastError);
            if (m_InitCallback) m_InitCallback(false, m_LastError);
            return false;
        }

        m_Stats.initStartTime = std::chrono::steady_clock::now();

        try
        {
            DeviceType deviceType = ConvertToDeviceType(m_PreferredAPI);
            m_Device = Device::Create(deviceType, m_Window, m_Width, m_Height);

            if (!m_Device)
            {
                m_LastError = "Failed to create graphics device";
                EC_CORE_ERROR(m_LastError);
                if (m_InitCallback) m_InitCallback(false, m_LastError);
                return false;
            }

            // Force initialization if device supports it
            bool initSuccess = m_Device->EnsureInitialized();
            
            m_Stats.initEndTime = std::chrono::steady_clock::now();
            m_Stats.isInitialized = initSuccess;
            
            if (initSuccess)
            {
                m_CurrentAPI = m_PreferredAPI;
                EC_CORE_INFO("Graphics device initialized successfully in {:.2f}ms", 
                           m_Stats.GetInitializationTimeMs());
                if (m_InitCallback) m_InitCallback(true, "");
            }
            else
            {
                m_LastError = "Graphics device initialization failed";
                EC_CORE_ERROR(m_LastError);
                if (m_InitCallback) m_InitCallback(false, m_LastError);
            }

            return initSuccess;
        }
        catch (const std::exception& e)
        {
            m_LastError = "Exception during graphics initialization: " + std::string(e.what());
            EC_CORE_ERROR(m_LastError);
            
            m_Stats.initEndTime = std::chrono::steady_clock::now();
            m_Stats.isInitialized = false;
            
            if (m_InitCallback) m_InitCallback(false, m_LastError);
            return false;
        }
    }

    bool GraphicsContext::IsInitialized() const
    {
        std::lock_guard<std::mutex> lock(m_ContextMutex);
        return m_Device && m_Device->IsInitialized();
    }

    bool GraphicsContext::EnsureInitialized()
    {
        std::lock_guard<std::mutex> lock(m_ContextMutex);
        
        if (IsInitialized())
        {
            return true;
        }

        if (!m_InitializationAttempted)
        {
            EC_CORE_ERROR("Graphics context not properly initialized. Call Initialize() first.");
            return false;
        }

        return InitializeDevice();
    }

    void GraphicsContext::Shutdown()
    {
        std::lock_guard<std::mutex> lock(m_ContextMutex);
        
        if (m_Device)
        {
            EC_CORE_INFO("Shutting down graphics context...");
            m_Device->Shutdown();
            m_Device.reset();
        }

        m_CurrentAPI = GraphicsAPI::None;
        m_Stats = GraphicsContextStats{}; // Reset stats
        m_InitializationAttempted = false;
        
        EC_CORE_INFO("Graphics context shutdown completed");
    }

    Device* GraphicsContext::GetDevice()
    {
        if (m_LazyLoadingEnabled && !IsInitialized())
        {
            if (!EnsureInitialized())
            {
                EC_CORE_ERROR("Failed to initialize graphics device on demand");
                return nullptr;
            }
        }
        
        return m_Device.get();
    }

    const Device* GraphicsContext::GetDevice() const
    {
        return m_Device.get();
    }

    bool GraphicsContext::SupportsAPI(GraphicsAPI api) const
    {
        switch (api)
        {
            case GraphicsAPI::Vulkan:
                // Check if Vulkan is available (basic check)
                return true; // For now, assume Vulkan is always available
            case GraphicsAPI::DirectX12:
                // Check if DirectX 12 is available
                return false; // Not implemented yet
            case GraphicsAPI::OpenGL:
                // Check if OpenGL is available
                return false; // Not implemented yet
            default:
                return false;
        }
    }

    std::vector<GraphicsAPI> GraphicsContext::GetAvailableAPIs() const
    {
        std::vector<GraphicsAPI> apis;
        
        if (SupportsAPI(GraphicsAPI::Vulkan))
            apis.push_back(GraphicsAPI::Vulkan);
        
        if (SupportsAPI(GraphicsAPI::DirectX12))
            apis.push_back(GraphicsAPI::DirectX12);
        
        if (SupportsAPI(GraphicsAPI::OpenGL))
            apis.push_back(GraphicsAPI::OpenGL);
        
        return apis;
    }

    std::string GraphicsContext::GetAPIName(GraphicsAPI api) const
    {
        switch (api)
        {
            case GraphicsAPI::Vulkan: return "Vulkan";
            case GraphicsAPI::DirectX12: return "DirectX 12";
            case GraphicsAPI::OpenGL: return "OpenGL";
            default: return "Unknown";
        }
    }

    DeviceType GraphicsContext::ConvertToDeviceType(GraphicsAPI api) const
    {
        switch (api)
        {
            case GraphicsAPI::Vulkan: return DeviceType::Vulkan;
            default:
                EC_CORE_ASSERT(false, "Unsupported graphics API");
                return DeviceType::Vulkan;
        }
    }
} 