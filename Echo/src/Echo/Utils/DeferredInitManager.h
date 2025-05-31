#pragma once
#include <functional>
#include <vector>

namespace Echo {
class DeferredInitManager {
public:
    static void Enqueue(std::function<void()> task);
    static void Process(std::function<bool()> isReady);
    static bool HasPending();
private:
    static std::vector<std::function<void()>> s_DeferredTasks;
    static bool s_Processing;
};
} 