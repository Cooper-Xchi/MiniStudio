#include "Application.h"
#include "platform/GlfwWindow.h"
#include "render/ShaderProgram.h"

constexpr const char* vertex_source = R"(#version 410 core
layout(location = 0) in vec3 position;

void main() {
    gl_Position = vec4(position, 1.0);
}
)";

constexpr const char* fragment_source = R"(#version 410 core
out vec4 fragment_color;

void main() {
    fragment_color = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

int Application::Run() {
    if (!window_.Initialize(1280,960,"MiniStudio"))return 1;
    if (!shader_program_.Initialize(vertex_source, fragment_source)) return 1;
    shader_program_.Use();
    while (!window_.ShouldClose()) {
        window_.PollEvents();
        if (window_.IsEscapePressed()) {
            window_.RequestClose();
        }

    }
    return 0;
}
