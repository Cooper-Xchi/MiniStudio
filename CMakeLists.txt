cmake_minimum_required(VERSION 3.25)

project(MiniStudio LANGUAGES CXX)

add_executable(ministudio
        src/main.cpp
)

option(MINISTUDIO_ENABLE_SANITIZERS
        "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
        OFF)

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
