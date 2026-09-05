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
layout(location = 2) in vec2 uv;
out vec3 vertex_color;
out vec2 uv_coord;

void main() {
    vertex_color = color;
    uv_coord = uv;
    gl_Position = vec4(position, 1.0);
}
)";

    const char* fragment_source = R"(#version 410 core
    out vec4 fragment_color;
    in vec3 vertex_color;
    in vec2 uv_coord;
uniform sampler2D texture_sampler;

void main() {
    fragment_color = texture(texture_sampler, uv_coord);
}
)";

    const float triangle_vertices[] = {
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    };

    const unsigned int indexes[] = {
        0,1,2,
        2,3,0
    };

    const unsigned char rgba_array[] = {
            255 ,0   ,0   ,255,
            0   ,255 ,0   ,255,
            0   ,0   ,255 ,255,
            255 ,255 ,0   ,255,
    };

    if (!shader_program_.Initialize(vertex_source, fragment_source)) return false;
    if (!vertex_array_.Initialize(triangle_vertices,std::size(triangle_vertices),indexes,std::size(indexes))) return false;
    if (!texture_.Initialize(2, 2,rgba_array)) return false;


    shader_program_.Use();
    if (!shader_program_.SetInt("texture_sampler", 0)) {
        return false;
    }
    return true;
}

void Renderer::DrawFrame() {
    #ifndef NDEBUG
        OpenGLDebug::ClearErrors();
    #endif
    RenderCommand::Clear(0.36,0.5,0.6,1);
    shader_program_.Use();
    vertex_array_.Bind();
    texture_.Bind(0);
    RenderCommand::DrawIndexedTriangles(vertex_array_.IndexCount());
    #ifndef NDEBUG
        OpenGLDebug::CheckErrors("Renderer::DrawFrame");
    #endif
}
