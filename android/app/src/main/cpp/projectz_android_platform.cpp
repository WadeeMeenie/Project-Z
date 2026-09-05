#include "projectz_android_platform.h"

#include <atomic>

namespace {
std::atomic<std::int32_t> gAction{0};
std::atomic<float> gX{0.0f};
std::atomic<float> gY{0.0f};
std::atomic<bool> gDown{false};
} // namespace

namespace projectz::android {

void setInput(std::int32_t action, float x, float y) {
    gAction.store(action, std::memory_order_relaxed);
    gX.store(x, std::memory_order_relaxed);
    gY.store(y, std::memory_order_relaxed);
    gDown.store(action != 1 && action != 3 && action != 4, std::memory_order_relaxed);
}

InputState getInput() {
    return {
        gAction.load(std::memory_order_relaxed),
        gX.load(std::memory_order_relaxed),
        gY.load(std::memory_order_relaxed),
        gDown.load(std::memory_order_relaxed),
    };
}

} // namespace projectz::android
