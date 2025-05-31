#include "VulkanDeferredInitManager.h"
#include "Core/Application.h"
#include "Core/Window.h"
#include "Graphics/Device.h"

namespace Echo {

std::vector<std::function<void()>> VulkanDeferredInitManager::s_DeferredTasks;
bool VulkanDeferredInitManager::s_Processing = false;

void VulkanDeferredInitManager::Enqueue(std::function<void()> task) {
    s_DeferredTasks.push_back(task);
}

void VulkanDeferredInitManager::Process() {
    if (s_Processing) return; // Prevent recursion
    s_Processing = true;
    Application& app = Application::Get();
    Device* device = app.GetWindow().GetDevice();
    if (device && device->IsInitialized()) {
        for (auto& task : s_DeferredTasks) {
            task();
        }
        s_DeferredTasks.clear();
    }
    s_Processing = false;
}

bool VulkanDeferredInitManager::HasPending() {
    return !s_DeferredTasks.empty();
}

} 