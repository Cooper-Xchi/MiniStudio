cmake_minimum_required(VERSION 3.25)

project(MiniStudio LANGUAGES CXX)

add_executable(ministudio
        src/main.cpp
)

target_compile_features(ministudio PRIVATE cxx_std_20)

set_target_properties(ministudio PROPERTIES
        CXX_EXTENSIONS OFF
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(ministudio PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()