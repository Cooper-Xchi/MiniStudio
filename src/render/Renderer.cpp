//
// Created by 陈泓池 on 2026/9/4.
//

#include "Renderer.h"
#include "image/ImageLoader.h"
#include <iterator>
#include <glm/ext/matrix_clip_space.hpp>
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
uniform mat4 projection;
out vec2 uv_coord;

void main() {
    uv_coord = uv;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
)";

    const char* fragment_source = R"(#version 410 core
    out vec4 fragment_color;
    in vec2 uv_coord;
uniform sampler2D texture_sampler;

void main() {
    vec4 face;
    if(gl_FrontFacing){
    face = vec4(1.0,0.0,0.0,1.0);
}else{
    face = vec4(0.0,1.0,0.0,1.0);
}
    fragment_color = mix(texture(texture_sampler, uv_coord),face,0.5f);
}
)";

    const float cube_vertices[] = {
        // 正面 +Z：从立方体外部观察为逆时针
        -0.5f, -0.5f,  0.5f,  1, 0, 0,  0, 0,
         0.5f, -0.5f,  0.5f,  0, 1, 0,  1, 0,
         0.5f,  0.5f,  0.5f,  0, 0, 1,  1, 1,
        -0.5f,  0.5f,  0.5f,  1, 1, 1,  0, 1,

        //后面
         0.5f, -0.5f,  -0.5f,  1, 0, 0,  0, 0,
        -0.5f, -0.5f,  -0.5f,  0, 1, 0,  1, 0,
        -0.5f,  0.5f,  -0.5f,  0, 0, 1,  1, 1,
         0.5f,  0.5f,  -0.5f,  1, 1, 1,  0, 1,

        //左面
        -0.5f, -0.5f, -0.5f,  1, 0, 0,  0, 0,
        -0.5f, -0.5f,  0.5f,  0, 1, 0,  1, 0,
        -0.5f,  0.5f,  0.5f,  0, 0, 1,  1, 1,
        -0.5f,  0.5f, -0.5f,  1, 1, 1,  0, 1,

        //右面
         0.5f, -0.5f,  0.5f,  1, 0, 0,  0, 0,
         0.5f, -0.5f, -0.5f,  0, 1, 0,  1, 0,
         0.5f,  0.5f, -0.5f,  0, 0, 1,  1, 1,
         0.5f,  0.5f,  0.5f,  1, 1, 1,  0, 1,

        //上面
        -0.5f,  0.5f,  0.5f,  1, 0, 0,  0, 0,
         0.5f,  0.5f,  0.5f,  0, 1, 0,  1, 0,
         0.5f,  0.5f, -0.5f,  0, 0, 1,  1, 1,
        -0.5f,  0.5f, -0.5f,  1, 1, 1,  0, 1,

        //下面
        -0.5f, -0.5f, -0.5f,  1, 0, 0,  0, 0,
         0.5f, -0.5f, -0.5f,  0, 1, 0,  1, 0,
         0.5f, -0.5f,  0.5f,  0, 0, 1,  1, 1,
        -0.5f, -0.5f,  0.5f,  1, 1, 1,  0, 1,
    };

    const unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
        8, 9, 10,
        10, 11, 8,
        12, 13, 14,
        14, 15, 12,
        16,17, 18,
        18, 19, 16,
        20,21,22,
        22,23,20,
    };

    ImageData image;
    if (!LoadImageRgba("assets/textures/lesson21-quadrants.png",image)) {
        return false;
    }
    if (!shader_program_.Initialize(vertex_source, fragment_source)) return false;
    if (!vertex_array_.Initialize(cube_vertices,std::size(cube_vertices),indices,std::size(indices))) return false;
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

glm::mat4 CubeModel(float elapsed_seconds) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.1f, 0.0f, -2.0f));
    model = glm::rotate(model,glm::radians(90.0f * elapsed_seconds), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model,glm::radians(180.0f * elapsed_seconds), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model,glm::vec3(0.5f));
    return model;
}

bool Renderer::DrawFrame(
    const float elapsed_seconds,
    const int framebuffer_width,
    const int framebuffer_height
) {
    if (framebuffer_width <= 0 || framebuffer_height <= 0) {
        return true;
    }
    #ifndef NDEBUG
        OpenGLDebug::ClearErrors();
    #endif
    RenderCommand::SetGlobalDepth(true,true,RenderCommand::DepthCompare::LESS);
    RenderCommand::SetGlobalCullFace(true,RenderCommand::CullFace::Back,RenderCommand::FrontFaceWinding::CounterClockwise);
    RenderCommand::Clear(0.36,0.5,0.6,1);
    shader_program_.Use();
    const float aspect =
        static_cast<float>(framebuffer_width) /
        static_cast<float>(framebuffer_height);
    const glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        aspect,
        0.1f,
        100.0f
    );
    if (!shader_program_.SetMat4("projection", projection)) {
        return false;
    }
    if (!shader_program_.SetMat4("model",CubeModel(elapsed_seconds))) {
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
