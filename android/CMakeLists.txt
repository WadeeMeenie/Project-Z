cmake_minimum_required(VERSION 3.22.1)
project(projectz_android LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_Z_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." CACHE PATH "Project-Z source directory")

add_library(projectz_android SHARED
    app/src/main/cpp/projectz_android.cpp
)

target_include_directories(projectz_android PRIVATE
    "${PROJECT_Z_SOURCE_DIR}/include"
    "${PROJECT_Z_SOURCE_DIR}/src"
)

target_compile_definitions(projectz_android PRIVATE
    PLATFORM_ANDROID=1
    PLATFORM_N64=0
)

target_link_libraries(projectz_android
    android
    log
    EGL
    GLESv3
)
