#include "Application.h"

#include <iterator>

#include "platform/GlfwWindow.h"
#include "render/ShaderProgram.h"

constexpr const char* vertex_source = R"(#version 410 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec3 vertex_color;

void main() {
    gl_Position = vec4(position, 1.0);
    vertex_color = color;
}
)";

constexpr const char* fragment_source = R"(#version 410 core
out vec4 fragment_color;
in vec3 vertex_color;

void main() {
    fragment_color = vec4(vertex_color, 1.0);
}
)";

constexpr float triangle_vertices[] = {
    -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
};

int Application::Run() {
    if (!window_.Initialize(1280,960,"MiniStudio"))return 1;
    if (!shader_program_.Initialize(vertex_source, fragment_source)) return 1;
    if (!vertex_array_.Initialize(triangle_vertices,std::size(triangle_vertices))) return 1;
    shader_program_.Use();
    vertex_array_.Bind();
    while (!window_.ShouldClose()) {
        window_.PollEvents();
        if (window_.IsEscapePressed()) {
            window_.RequestClose();
        }

    }
    return 0;
}
