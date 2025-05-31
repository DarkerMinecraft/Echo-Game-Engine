#pragma once
#include <functional>
#include <vector>

namespace Echo {
class VulkanDeferredInitManager {
public:
    static void Enqueue(std::function<void()> task);
    static void Process();
    static bool HasPending();
private:
    static std::vector<std::function<void()>> s_DeferredTasks;
    static bool s_Processing;
};
} 