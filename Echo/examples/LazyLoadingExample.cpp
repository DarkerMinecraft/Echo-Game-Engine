// LazyLoadingExample.cpp
// Demonstrates how to use Echo Engine's new lazy loading graphics system

#include <Echo.h>
#include <iostream>
#include <chrono>

using namespace Echo;

class LazyLoadingExampleLayer : public Layer
{
public:
    LazyLoadingExampleLayer() : Layer("LazyLoadingExample")
    {
        
    }

    virtual void OnAttach() override
    {
        EC_PROFILE_FUNCTION();

        // Setup graphics context callbacks to monitor initialization
        auto& graphicsManager = GraphicsManager::GetInstance();
        auto* context = graphicsManager.GetContext();
        
        if (context)
        {
            // Set up initialization callback
            context->SetInitializationCallback([](bool success, const std::string& errorMessage) {
                if (success)
                {
                    EC_INFO("Graphics initialized successfully!");
                }
                else
                {
                    EC_ERROR("Graphics initialization failed: {}", errorMessage);
                }
            });

            // Set up resource tracking callback
            context->SetResourceCallback([](const std::string& resourceName, bool created) {
                if (created)
                {
                    EC_INFO("Resource created: {}", resourceName);
                }
                else
                {
                    EC_INFO("Resource destroyed: {}", resourceName);
                }
            });

            // At this point, graphics are NOT initialized yet!
            // They will initialize when first needed
            EC_INFO("Graphics context created, but not initialized");
        }
    }

    virtual void OnUpdate(Timestep ts) override
    {
        EC_PROFILE_FUNCTION();

        // Simulate some non-graphics work that happens every frame
        // This demonstrates that the application can run without graphics initialization
        
        static bool firstGraphicsCall = true;
        static auto startTime = std::chrono::steady_clock::now();
        
        // After 2 seconds, make the first graphics call to trigger lazy initialization
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        
        if (elapsed > 2000 && firstGraphicsCall)
        {
            EC_INFO("Making first graphics call - this will trigger Vulkan initialization");
            
            // This call will trigger the lazy initialization of Vulkan
            auto& graphicsManager = GraphicsManager::GetInstance();
            Device* device = graphicsManager.GetDevice();
            
            if (device)
            {
                EC_INFO("Graphics device obtained successfully!");
                
                // Print initialization statistics
                auto* context = graphicsManager.GetContext();
                if (context)
                {
                    const auto& stats = context->GetStats();
                    EC_INFO("Graphics initialization took {:.2f}ms", stats.GetInitializationTimeMs());
                }
                
                // Now you can use the device for graphics operations
                // For example, create buffers, textures, etc.
                DemonstrateResourceCreation(device);
            }
            else
            {
                EC_ERROR("Failed to obtain graphics device!");
            }
            
            firstGraphicsCall = false;
        }
    }

    virtual void OnImGuiRender() override
    {
        EC_PROFILE_FUNCTION();
        
        // ImGui demonstration of lazy loading system
        ImGui::Begin("Lazy Loading Demo");
        
        auto& graphicsManager = GraphicsManager::GetInstance();
        auto* context = graphicsManager.GetContext();
        
        if (context)
        {
            // Display current state
            ImGui::Text("Graphics API: %s", context->GetAPIName(context->GetCurrentAPI()).c_str());
            ImGui::Text("Lazy Loading: %s", context->IsLazyLoadingEnabled() ? "Enabled" : "Disabled");
            ImGui::Text("Initialized: %s", context->IsInitialized() ? "Yes" : "No");
            
            // Display statistics
            const auto& stats = context->GetStats();
            if (stats.isInitialized)
            {
                ImGui::Separator();
                ImGui::Text("Initialization Time: %.2fms", stats.GetInitializationTimeMs());
                ImGui::Text("Resources Created: %zu", stats.resourcesCreated);
                ImGui::Text("Resources Destroyed: %zu", stats.resourcesDestroyed);
            }
            
            // Control buttons
            ImGui::Separator();
            if (ImGui::Button("Force Initialize Graphics"))
            {
                if (!context->IsInitialized())
                {
                    EC_INFO("Manually forcing graphics initialization...");
                    context->EnsureInitialized();
                }
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Toggle Lazy Loading"))
            {
                context->SetLazyLoadingEnabled(!context->IsLazyLoadingEnabled());
            }
        }
        
        ImGui::End();
    }

private:
    void DemonstrateResourceCreation(Device* device)
    {
        EC_PROFILE_FUNCTION();
        
        // Example of creating graphics resources after lazy initialization
        // This demonstrates the resource tracking system
        
        auto& graphicsManager = GraphicsManager::GetInstance();
        auto* context = graphicsManager.GetContext();
        
        if (context)
        {
            EC_INFO("Demonstrating resource creation...");
            
            // Create a simple buffer
            // Note: This is a simplified example - actual implementation would vary
            try
            {
                // Simulate resource creation through the context's tracking system
                context->DestroyResource("DemoBuffer");  // This would track the destruction
                
                EC_INFO("Resource demonstration completed");
            }
            catch (const std::exception& e)
            {
                EC_ERROR("Error during resource demonstration: {}", e.what());
            }
        }
    }
};

class LazyLoadingExampleApp : public Application
{
public:
    LazyLoadingExampleApp() : Application("assets/", "Lazy Loading Example")
    {
        PushLayer(new LazyLoadingExampleLayer());
    }

    ~LazyLoadingExampleApp()
    {
        // Graphics will automatically shut down when the application closes
    }
};

// Factory function for creating the application
Application* Echo::CreateApplication()
{
    return new LazyLoadingExampleApp();
}

// Main function demonstrating the lazy loading system
/*
int main()
{
    // Initialize logging
    Echo::Log::Init();
    
    EC_INFO("=== Echo Engine Lazy Loading Demo ===");
    EC_INFO("This demo shows how the graphics system initializes on-demand");
    
    try
    {
        // Create and run the application
        auto app = Echo::CreateApplication();
        
        EC_INFO("Application created - graphics will initialize when first needed");
        
        // Run the application
        app->Run();
        
        // Cleanup
        delete app;
        
        EC_INFO("Application completed successfully");
    }
    catch (const std::exception& e)
    {
        EC_ERROR("Application error: {}", e.what());
        return -1;
    }
    catch (...)
    {
        EC_ERROR("Unknown application error");
        return -1;
    }
    
    return 0;
}
*/ 