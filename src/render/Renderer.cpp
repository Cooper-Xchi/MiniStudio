//
// Created by 陈泓池 on 2026/9/4.
//

#include "Renderer.h"
#include "image/ImageLoader.h"
#include <iterator>
#include <glm/ext/matrix_transform.hpp>

#include "OpenGLDebug.h"
#include "RenderCommand.h"

bool Renderer::Initialize() {

    const char* vertex_source = R"(#version 410 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;
uniform mat4 model;
uniform mat4 view;
out vec2 uv_coord;

void main() {
    uv_coord = uv;
    gl_Position = view * model * vec4(position, 1.0);
}
)";

    const char* fragment_source = R"(#version 410 core
    out vec4 fragment_color;
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



    ImageData image;
    if (!LoadImageRgba("assets/textures/lesson21-quadrants.png",image)) {
        return false;
    }

    if (!shader_program_.Initialize(vertex_source, fragment_source)) return false;
    if (!vertex_array_.Initialize(triangle_vertices,std::size(triangle_vertices),indexes,std::size(indexes))) return false;
    if (!texture_.Initialize(image.width, image.height,image.rgba_pixels.data())) return false;


    shader_program_.Use();

    glm::vec3 camera_position = glm::vec3(0.25,0,0);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, -camera_position);
    if (!shader_program_.SetInt("texture_sampler", 0)) {
        return false;
    }

    if (!shader_program_.SetMat4("view",view)) {
        return false;
    }
    return true;
}

bool Renderer::DrawFrame(const float elapsed_seconds) {
    #ifndef NDEBUG
        OpenGLDebug::ClearErrors();
    #endif
    RenderCommand::Clear(0.36,0.5,0.6,1);
    shader_program_.Use();

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.25f, 0.0f, 0.0f));
    model = glm::rotate(model,glm::radians(90.0f * elapsed_seconds), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model,glm::vec3(0.5f));
    if (!shader_program_.SetMat4("model",model)) {
        return false;
    }
    vertex_array_.Bind();
    texture_.Bind(0);
    RenderCommand::DrawIndexedTriangles(vertex_array_.IndexCount());
    #ifndef NDEBUG
        return OpenGLDebug::CheckErrors("Renderer::DrawFrame");
    #endif
    return true;
}
