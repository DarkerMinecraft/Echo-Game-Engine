#include "DeferredInitManager.h"
#include "Core/Log.h"

namespace Echo {

std::vector<std::function<void()>> DeferredInitManager::s_DeferredTasks;
bool DeferredInitManager::s_Processing = false;

void DeferredInitManager::Enqueue(std::function<void()> task) {
    EC_CORE_INFO("[DeferredInitManager] Enqueuing deferred task. Total pending: {}", s_DeferredTasks.size() + 1);
    s_DeferredTasks.push_back(task);
}

void DeferredInitManager::Process(std::function<bool()> isReady) {
    if (s_Processing) return; // Prevent recursion
    s_Processing = true;
    if (isReady && isReady()) {
        EC_CORE_INFO("[DeferredInitManager] Processing {} deferred tasks.", s_DeferredTasks.size());
        for (auto& task : s_DeferredTasks) {
            task();
        }
        s_DeferredTasks.clear();
    }
    s_Processing = false;
}

bool DeferredInitManager::HasPending() {
    return !s_DeferredTasks.empty();
}

} 