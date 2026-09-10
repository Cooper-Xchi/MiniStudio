//
// Created by 陈泓池 on 2026/9/4.
//

#include "Renderer.h"
#include <algorithm>
#include <array>

#include "image/ImageLoader.h"
#include <iterator>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "OpenGLDebug.h"
#include "RenderCommand.h"

namespace {

    ImageData CreateTranslucentImage() {
        ImageData image;
        int width = 16 , height = 16;
        image.width = width;
        image.height = height;
        image.rgba_pixels.resize(width * height * 4);
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                int index = x  * width + y;
                image.rgba_pixels[4*index] = 255;
                image.rgba_pixels[4*index+1] = 255;
                image.rgba_pixels[4*index+2] = 255;
                image.rgba_pixels[4*index+3] = 128;

            }
        }
        return image;

    }

    struct TransparentDrawItem {
        glm::mat4 model;
        glm::vec4 tint;
        float view_z;
    };
}

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
uniform vec4 tint;

void main() {
    vec4 texel = texture(texture_sampler, uv_coord);
    fragment_color = tint * texel;
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

    const float translucent_vertices[] = {
        // 正面 +Z：从立方体外部观察为逆时针
        -0.8f, -0.8f,  0.0f,  1, 0, 0,  0, 0,
         0.8f, -0.8f,  0.0f,  0, 1, 0,  1, 0,
         0.8f,  0.8f,  0.0f,  0, 0, 1,  1, 1,
        -0.8f,  0.8f,  0.0f,  1, 1, 1,  0, 1,
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

    const unsigned int translucent_indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    ImageData image;
    const ImageData translucent_image = CreateTranslucentImage();
    if (!LoadImageRgba("assets/textures/lesson21-quadrants.png",image)) {
        return false;
    }
    if (!shader_program_.Initialize(vertex_source, fragment_source)) return false;
    if (!cube_vertex_array_.Initialize(cube_vertices,std::size(cube_vertices),indices,std::size(indices))) return false;
    if (!translucent_vertex_array_.Initialize(translucent_vertices,std::size(translucent_vertices),translucent_indices,std::size(translucent_indices))) return false;
    if (!cube_texture_.Initialize(image.width, image.height,image.rgba_pixels.data())) return false;
    if (!translucent_texture_.Initialize(translucent_image.width, translucent_image.height,translucent_image.rgba_pixels.data())) return false;
    shader_program_.Use();
    if (!shader_program_.SetInt("texture_sampler", 0)) {
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

glm::mat4 TranslucentModel() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.1f, 0.0f, -1.2f));
    model = glm::scale(model,glm::vec3(1.0f));
    return model;
}



bool Renderer::DrawFrame(
    const float elapsed_seconds,
    const int framebuffer_width,
    const int framebuffer_height,
    const glm::mat4& view
) {
    //检查
    if (framebuffer_width <= 0 || framebuffer_height <= 0) {
        return true;
    }
    //Debug/Release 模式
    #ifndef NDEBUG
        OpenGLDebug::ClearErrors();
    #endif

    //设置全局状态
    RenderCommand::SetGlobalDepth(true,true,RenderCommand::DepthCompare::LESS);
    RenderCommand::SetGlobalCullFace(true,RenderCommand::CullFace::Back,RenderCommand::FrontFaceWinding::CounterClockwise);
    RenderCommand::Clear(0.36,0.5,0.6,1);

    //开始

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
    if (!shader_program_.SetMat4("view", view)) {
        return false;
    }
    if (!shader_program_.SetMat4("projection", projection)) {
        return false;
    }
    std::array<TransparentDrawItem, 2> items{};
    TransparentDrawItem item;
    TransparentDrawItem item2;
    item.model = glm::translate(glm::mat4(1.0f),glm::vec3(0.2f,0.2f,-1.0f));
    item.tint = glm::vec4(1.0f,0.0f,0.0f,1.0f);
    item2.model = glm::translate(glm::mat4(1.0f),glm::vec3(-0.2f,-0.2f,-1.50f));
    item2.tint = glm::vec4(0.0f,0.0f,1.0f,1.0f);
    const glm::vec4 center_view =
    view * item.model * glm::vec4(0, 0, 0, 1);
    const glm::vec4 center_view2 =
    view * item2.model * glm::vec4(0, 0, 0, 1);
    item.view_z = center_view.z;
    item2.view_z = center_view2.z;
    // 将 center_view.z 保存到 item.view_z
    items[0] = item;
    items[1] = item2;
    std::sort(items.begin(),items.end(),
    [](const TransparentDrawItem& a, const TransparentDrawItem& b) {
        return a.view_z < b.view_z;
    });
    cube_vertex_array_.Bind();
    cube_texture_.Bind(0);
    if (!shader_program_.SetMat4("model",CubeModel(elapsed_seconds))) {
        return false;
    }
    if (!shader_program_.SetVec4("tint",glm::vec4(1.0f,1.0f,1.0f,1.0f))) {
        return false;
    }
    RenderCommand::DrawIndexedTriangles(cube_vertex_array_.IndexCount());
    for (auto& item : items) {
        RenderCommand::SetDepthWriteEnabled(false);
        RenderCommand::SetBlendingEnabled(true);
        RenderCommand::SetBlendFunction(RenderCommand::BlendFactor::SourceAlpha,RenderCommand::BlendFactor::OneMinusSourceAlpha);
        RenderCommand::SetFaceCullingEnabled(false);
        shader_program_.Use();
        translucent_vertex_array_.Bind();
        translucent_texture_.Bind(0);
        if (!shader_program_.SetMat4("model",item.model)) {
            return false;
        }
        if (!shader_program_.SetVec4("tint",item.tint)) {
            return false;
        }
        RenderCommand::DrawIndexedTriangles(translucent_vertex_array_.IndexCount());
    }

    RenderCommand::SetDepthWriteEnabled(true);
    RenderCommand::SetBlendingEnabled(false);
    RenderCommand::SetFaceCullingEnabled(true);

    #ifndef NDEBUG
        return OpenGLDebug::CheckErrors("Renderer::DrawFrame");
    #endif
    return true;
}
