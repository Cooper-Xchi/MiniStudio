cmake_minimum_required(VERSION 3.25)

project(MiniStudio LANGUAGES CXX)

add_executable(ministudio
        src/main.cpp
        src/app/Application.cpp
        src/platform/GlfwWindow.cpp
        src/render/ShaderProgram.cpp
        src/render/VertexArray.cpp
)

option(MINISTUDIO_ENABLE_SANITIZERS
        "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
        OFF)
target_include_directories(ministudio PRIVATE src)
target_compile_features(ministudio PRIVATE cxx_std_20)

set_target_properties(ministudio PROPERTIES
        CXX_EXTENSIONS OFF
)

find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(ministudio PRIVATE glfw OpenGL::GL)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(ministudio PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
    if(MINISTUDIO_ENABLE_SANITIZERS)
        target_compile_options(ministudio PRIVATE
                -fsanitize=address,undefined
                -fno-omit-frame-pointer
        )

        target_link_options(ministudio PRIVATE
                -fsanitize=address,undefined
        )
    endif()
endif()
if(APPLE)
    target_compile_definitions(ministudio PRIVATE
            GL_SILENCE_DEPRECATION
    )
endif()
