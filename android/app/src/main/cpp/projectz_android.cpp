#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <jni.h>

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

#include "projectz_android_platform.h"

#define LOG_TAG "Project-Z"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {
ANativeWindow* gWindow = nullptr;
std::mutex gSurfaceMutex;
std::condition_variable gSurfaceCondition;
std::thread gRenderThread;
bool gRenderThreadRunning = false;

EGLDisplay gDisplay = EGL_NO_DISPLAY;
EGLSurface gSurface = EGL_NO_SURFACE;
EGLContext gContext = EGL_NO_CONTEXT;

void destroyEgl() {
    if (gDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(gDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (gSurface != EGL_NO_SURFACE) {
            eglDestroySurface(gDisplay, gSurface);
        }
        if (gContext != EGL_NO_CONTEXT) {
            eglDestroyContext(gDisplay, gContext);
        }
        eglTerminate(gDisplay);
    }
    gDisplay = EGL_NO_DISPLAY;
    gSurface = EGL_NO_SURFACE;
    gContext = EGL_NO_CONTEXT;
}

bool createEgl(ANativeWindow* window) {
    gDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (gDisplay == EGL_NO_DISPLAY || !eglInitialize(gDisplay, nullptr, nullptr)) {
        LOGE("eglInitialize failed");
        destroyEgl();
        return false;
    }

    const EGLint configAttrs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE,
    };
    EGLConfig config = nullptr;
    EGLint count = 0;
    if (!eglChooseConfig(gDisplay, configAttrs, &config, 1, &count) || count == 0) {
        LOGE("eglChooseConfig failed");
        destroyEgl();
        return false;
    }

    const EGLint contextAttrs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    gContext = eglCreateContext(gDisplay, config, EGL_NO_CONTEXT, contextAttrs);
    if (gContext == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        destroyEgl();
        return false;
    }

    gSurface = eglCreateWindowSurface(gDisplay, config, window, nullptr);
    if (gSurface == EGL_NO_SURFACE || !eglMakeCurrent(gDisplay, gSurface, gSurface, gContext)) {
        LOGE("EGL window surface/current context failed");
        destroyEgl();
        return false;
    }

    eglSwapInterval(gDisplay, 1);
    glViewport(0, 0, ANativeWindow_getWidth(window), ANativeWindow_getHeight(window));
    LOGI("OpenGL ES renderer ready: %s", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    return true;
}

void renderFrame() {
    if (gDisplay == EGL_NO_DISPLAY || gSurface == EGL_NO_SURFACE) {
        return;
    }

    const auto input = projectz::android::getInput();
    const float intensity = input.down ? 0.055f : 0.035f;
    glClearColor(intensity, 0.055f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    eglSwapBuffers(gDisplay, gSurface);
}

void renderThreadMain() {
    std::unique_lock<std::mutex> lock(gSurfaceMutex);
    while (gRenderThreadRunning) {
        gSurfaceCondition.wait(lock, [] { return !gRenderThreadRunning || gWindow != nullptr; });
        if (!gRenderThreadRunning) {
            break;
        }

        ANativeWindow* window = gWindow;
        lock.unlock();

        if (createEgl(window)) {
            constexpr auto frameDuration = std::chrono::microseconds(16667);
            while (gRenderThreadRunning) {
                {
                    std::lock_guard<std::mutex> guard(gSurfaceMutex);
                    if (gWindow != window) {
                        break;
                    }
                }

                const auto frameStart = std::chrono::steady_clock::now();
                renderFrame();
                const auto elapsed = std::chrono::steady_clock::now() - frameStart;
                if (elapsed < frameDuration) {
                    std::this_thread::sleep_for(frameDuration - elapsed);
                }
            }
            destroyEgl();
        }

        lock.lock();
    }
}

void stopRenderThread() {
    {
        std::lock_guard<std::mutex> lock(gSurfaceMutex);
        gRenderThreadRunning = false;
    }
    gSurfaceCondition.notify_all();
    if (gRenderThread.joinable()) {
        gRenderThread.join();
    }
}
} // namespace

extern "C" JNIEXPORT void JNICALL
Java_com_projectz_android_MainActivity_nativeSetSurface(JNIEnv* env, jobject, jobject surface) {
    std::lock_guard<std::mutex> lock(gSurfaceMutex);

    if (surface == nullptr) {
        if (gWindow != nullptr) {
            ANativeWindow_release(gWindow);
            gWindow = nullptr;
        }
        gSurfaceCondition.notify_all();
        return;
    }

    ANativeWindow* newWindow = ANativeWindow_fromSurface(env, surface);
    if (newWindow == nullptr) {
        LOGE("ANativeWindow_fromSurface failed");
        return;
    }

    if (gWindow != nullptr) {
        ANativeWindow_release(gWindow);
    }
    gWindow = newWindow;
    if (!gRenderThreadRunning) {
        gRenderThreadRunning = true;
        gRenderThread = std::thread(renderThreadMain);
    }
    gSurfaceCondition.notify_all();
    LOGI("Android surface attached: %dx%d", ANativeWindow_getWidth(gWindow), ANativeWindow_getHeight(gWindow));
}

extern "C" JNIEXPORT void JNICALL
Java_com_projectz_android_MainActivity_nativeOnInput(JNIEnv*, jobject, jint action, jfloat x, jfloat y) {
    projectz::android::setInput(action, x, y);
}

extern "C" JNIEXPORT void JNICALL
Java_com_projectz_android_MainActivity_nativeShutdown(JNIEnv*, jobject) {
    stopRenderThread();

    std::lock_guard<std::mutex> lock(gSurfaceMutex);
    if (gWindow != nullptr) {
        ANativeWindow_release(gWindow);
        gWindow = nullptr;
    }
}
