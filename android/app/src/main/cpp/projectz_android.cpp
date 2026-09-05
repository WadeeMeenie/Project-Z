#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <jni.h>

#define LOG_TAG "Project-Z"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {
ANativeWindow* gWindow = nullptr;
EGLDisplay gDisplay = EGL_NO_DISPLAY;
EGLSurface gSurface = EGL_NO_SURFACE;
EGLContext gContext = EGL_NO_CONTEXT;

void destroyEgl() {
    if (gDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(gDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (gSurface != EGL_NO_SURFACE) eglDestroySurface(gDisplay, gSurface);
        if (gContext != EGL_NO_CONTEXT) eglDestroyContext(gDisplay, gContext);
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
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24, EGL_NONE
    };
    EGLConfig config = nullptr;
    EGLint count = 0;
    if (!eglChooseConfig(gDisplay, configAttrs, &config, 1, &count) || count == 0) {
        LOGE("eglChooseConfig failed");
        destroyEgl();
        return false;
    }

    const EGLint contextAttrs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
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
    if (gDisplay == EGL_NO_DISPLAY || gSurface == EGL_NO_SURFACE) return;
    glClearColor(0.035f, 0.055f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    eglSwapBuffers(gDisplay, gSurface);
}
}

extern "C" JNIEXPORT void JNICALL
Java_com_projectz_android_MainActivity_nativeSetSurface(JNIEnv*, jobject, jobject surface) {
    destroyEgl();
    if (gWindow != nullptr) {
        ANativeWindow_release(gWindow);
        gWindow = nullptr;
    }
    if (surface == nullptr) {
        LOGI("Android surface detached");
        return;
    }
    gWindow = ANativeWindow_fromSurface(nullptr, surface);
    if (gWindow == nullptr) return;
    LOGI("Android surface attached: %dx%d", ANativeWindow_getWidth(gWindow), ANativeWindow_getHeight(gWindow));
    if (createEgl(gWindow)) renderFrame();
}

extern "C" JNIEXPORT void JNICALL
Java_com_projectz_android_MainActivity_nativeOnInput(JNIEnv*, jobject, jint action, jfloat x, jfloat y) {
    LOGI("touch action=%d x=%.1f y=%.1f", action, x, y);
}
