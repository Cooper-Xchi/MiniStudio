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
        src/io/TextFileLoader.h
        src/io/TextFileLoader.cpp
        src/render/Material.h
        src/mesh/GltfMeshLoader.h
)

add_executable(ministudio src/main.cpp ${MINISTUDIO_SOURCES}
        src/mesh/GltfMeshLoader.h)

option(MINISTUDIO_ENABLE_SANITIZERS
        "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
        OFF)
option(MINISTUDIO_BUILD_TESTS "Build opt-in OpenGL integration checks (requires a display)" OFF)
option(MINISTUDIO_BUILD_GLTF_EXERCISE "Build the lesson 63 CPU glTF checks" OFF)

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

add_custom_target(ministudio_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:ministudio>/assets"
    COMMENT "Copying MiniStudio assets"
    VERBATIM
)

foreach(target IN LISTS MINISTUDIO_TARGETS)
    add_dependencies(${target} ministudio_assets)
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
endforeach()

# CPU-only checks stay independent of the renderer and its OpenGL test option.
if(MINISTUDIO_BUILD_GLTF_EXERCISE)
    enable_testing()
    add_executable(ministudio_gltf_exercise
        tests/GltfPrimitiveExercise.cpp
        src/mesh/GltfMeshLoader.cpp
    )
    add_dependencies(ministudio_gltf_exercise ministudio_assets)
    target_include_directories(ministudio_gltf_exercise PRIVATE src)
    target_include_directories(ministudio_gltf_exercise SYSTEM PRIVATE third_party/tinygltf)
    target_link_libraries(ministudio_gltf_exercise PRIVATE glm::glm)
    target_compile_features(ministudio_gltf_exercise PRIVATE cxx_std_20)
    set_target_properties(ministudio_gltf_exercise PROPERTIES CXX_EXTENSIONS OFF)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(ministudio_gltf_exercise PRIVATE -Wall -Wextra -Wpedantic)
        if(MINISTUDIO_ENABLE_SANITIZERS)
            target_compile_options(ministudio_gltf_exercise PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
            target_link_options(ministudio_gltf_exercise PRIVATE -fsanitize=address,undefined)
        endif()
    endif()
    if(MSVC)
        target_compile_options(ministudio_gltf_exercise PRIVATE /utf-8)
    endif()
    add_test(NAME ministudio.gltf.primitive COMMAND ministudio_gltf_exercise)
    set_tests_properties(ministudio.gltf.primitive PROPERTIES
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:ministudio_gltf_exercise>"
        TIMEOUT 30
        LABELS "cpu;gltf"
    )
endif()
