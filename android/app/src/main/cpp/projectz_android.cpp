#include <android/native_window.h>
#include <android/log.h>
#include <jni.h>

#define LOG_TAG "Project-Z"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace {
ANativeWindow* gWindow = nullptr;
}

extern "C" JNIEXPORT void JNICALL
Java_com_projectz_android_MainActivity_nativeSetSurface(JNIEnv*, jobject, jobject surface) {
    if (gWindow != nullptr) {
        ANativeWindow_release(gWindow);
        gWindow = nullptr;
    }

    if (surface != nullptr) {
        gWindow = ANativeWindow_fromSurface(nullptr, surface);
        if (gWindow != nullptr) {
            LOGI("Android surface attached: %dx%d", ANativeWindow_getWidth(gWindow),
                 ANativeWindow_getHeight(gWindow));
        }
    } else {
        LOGI("Android surface detached");
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_projectz_android_MainActivity_nativeOnInput(JNIEnv*, jobject, jint action, jfloat x, jfloat y) {
    (void)action;
    (void)x;
    (void)y;
}
