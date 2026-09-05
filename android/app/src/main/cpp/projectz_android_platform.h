#ifndef PROJECTZ_ANDROID_PLATFORM_H
#define PROJECTZ_ANDROID_PLATFORM_H

#include <cstdint>

namespace projectz::android {

struct InputState {
    std::int32_t action;
    float x;
    float y;
    bool down;
};

void setInput(std::int32_t action, float x, float y);
InputState getInput();

} // namespace projectz::android

#endif
