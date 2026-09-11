cmake_minimum_required(VERSION 3.25)

project(MiniStudio VERSION 0.2.0 LANGUAGES CXX)

set(MINISTUDIO_SOURCES
        src/app/Application.cpp
        src/platform/GlfwWindow.cpp
        src/render/ShaderProgram.cpp
        src/render/VertexArray.cpp
        src/render/RenderCommand.cpp
        src/render/RenderCommand.h
        src/render/Renderer.cpp
        src/render/OpenGLDebug.cpp
        src/render/Texture2D.cpp
        src/render/Texture2D.h
        src/image/ImageLoader.cpp
        src/camera/Camera.cpp
        src/mesh/PrimitiveMeshes.cpp
)

add_executable(ministudio src/main.cpp ${MINISTUDIO_SOURCES})

option(MINISTUDIO_ENABLE_SANITIZERS
        "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
        OFF)
option(MINISTUDIO_BUILD_TESTS "Build opt-in OpenGL integration checks (requires a display)" OFF)

find_package(glfw3 3.4 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

if(WIN32)
    find_package(glad CONFIG REQUIRED)
endif()

set(MINISTUDIO_TARGETS ministudio)
if(MINISTUDIO_BUILD_TESTS)
    enable_testing()
    add_executable(ministudio_regression tests/V02Regression.cpp ${MINISTUDIO_SOURCES})
    add_dependencies(ministudio_regression ministudio)
    list(APPEND MINISTUDIO_TARGETS ministudio_regression)
    add_test(NAME ministudio.v02.regression COMMAND ministudio_regression)
    set_tests_properties(ministudio.v02.regression PROPERTIES
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:ministudio_regression>"
        TIMEOUT 60
        LABELS "graphics;integration"
    )
endif()

foreach(target IN LISTS MINISTUDIO_TARGETS)
    target_include_directories(${target} PRIVATE src)
    target_include_directories(${target} SYSTEM PRIVATE third_party/stb)
    target_compile_features(${target} PRIVATE cxx_std_20)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    target_link_libraries(${target} PRIVATE glfw glm::glm OpenGL::GL)
    if(WIN32)
        target_link_libraries(${target} PRIVATE glad::glad)
        target_compile_definitions(${target} PRIVATE GLFW_INCLUDE_NONE)
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        if(MINISTUDIO_ENABLE_SANITIZERS)
            target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(${target} PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
    if(MSVC)
        target_compile_options(${target} PRIVATE /utf-8)
    endif()
    if(APPLE)
        target_compile_definitions(${target} PRIVATE GL_SILENCE_DEPRECATION)
    endif()
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${target}>/assets"
        VERBATIM
        )
endforeach()
