//
// Created by 陈泓池 on 2026/9/4.
//

#include "Renderer.h"

#include <iterator>

#include "OpenGLDebug.h"
#include "RenderCommand.h"

bool Renderer::Initialize() {

    const char* vertex_source = R"(#version 410 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
out vec3 vertex_color;

void main() {
    gl_Position = vec4(position, 1.0);
    vertex_color = color;
}
)";

    const char* fragment_source = R"(#version 410 core
out vec4 fragment_color;
in vec3 vertex_color;

void main() {
    fragment_color = vec4(vertex_color, 1.0);
}
)";

    const float triangle_vertices[] = {
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,
    };

    const unsigned int indexes[] = {
        0,1,2,
        2,3,0
    };

    if (!shader_program_.Initialize(vertex_source, fragment_source)) return false;
    if (!vertex_array_.Initialize(triangle_vertices,std::size(triangle_vertices),indexes,std::size(indexes))) return false;
    return true;
}

void Renderer::DrawFrame() {
    #ifndef NDEBUG
        OpenGLDebug::ClearErrors();
    #endif
    RenderCommand::Clear(0.36,0.5,0.6,1);
    shader_program_.Use();
    vertex_array_.Bind();
    RenderCommand::DrawIndexedTriangles(vertex_array_.IndexCount());
    #ifndef NDEBUG
        OpenGLDebug::CheckErrors("Renderer::DrawFrame");
    #endif
}
