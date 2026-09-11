//
// Created by 陈泓池 on 2026/9/4.
//

#include "Renderer.h"
#include <algorithm>
#include <array>
#include <iostream>

#include "image/ImageLoader.h"
#include <iterator>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "OpenGLDebug.h"
#include "RenderCommand.h"
#include "io/TextFileLoader.h"
#include "mesh/MeshData.h"
#include "mesh/PrimitiveMeshes.h"

namespace {
    ImageData CreateTranslucentImage() {
        ImageData image;
        int width = 16, height = 16;
        image.width = width;
        image.height = height;
        image.rgba_pixels.resize(width * height * 4);
        for (int x = 0; x < height; x++) {
            for (int y = 0; y < width; y++) {
                int index = x * width + y;
                image.rgba_pixels[4 * index] = 255;
                image.rgba_pixels[4 * index + 1] = 255;
                image.rgba_pixels[4 * index + 2] = 255;
                image.rgba_pixels[4 * index + 3] = 128;
            }
        }
        return image;
    }

    struct TransparentDrawItem {
        glm::mat4 model;
        glm::vec4 tint;
        float view_z;
    };

    std::array<TransparentDrawItem, 2> CreateTransparentDrawItems(const glm::mat4 &view) {
        std::array<TransparentDrawItem, 2> items{
            {
                {
                    glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, -1.0f)),
                    glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.0f
                },
                {
                    glm::translate(glm::mat4(1.0f), glm::vec3(-0.2f, -0.2f, -1.5f)),
                    glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 0.0f
                }
            }
        };
        for (auto &item: items) {
            item.view_z = (view * item.model * glm::vec4(0, 0, 0, 1)).z;
        }
        std::sort(items.begin(), items.end(),
                  [](const TransparentDrawItem &a, const TransparentDrawItem &b) {
                      return a.view_z < b.view_z;
                  });
        return items;
    }

    glm::mat4 CubeModel(float elapsed_seconds) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 0.0f, -2.0f));
        model = glm::rotate(model, glm::radians(90.0f * elapsed_seconds), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(180.0f * elapsed_seconds), glm::vec3(1.0f, 0.0f, 0.0f));
        return glm::scale(model, glm::vec3(0.5f));
    }
}

bool Renderer::Initialize() {
    std::string vertex_source;
    std::string fragment_source;
    if (!LoadTextFile("assets/shaders/scene.vert", vertex_source) ||
        !LoadTextFile("assets/shaders/scene.frag", fragment_source)) {
        return false;
    }

    ImageData image;
    if (!LoadImageRgba("assets/textures/lesson21-quadrants.png", image)) {
        return false;
    }
    const ImageData translucent_image = CreateTranslucentImage();
    const MeshData cube = CreateTexturedCubeMesh();
    if (!cube_vertex_array_.Initialize(cube)) return false;
    if (!cube_texture_.Initialize(image.width, image.height, image.rgba_pixels.data())) return false;
    const MeshData plane = CreateTexturedPlaneMesh();
    if (!translucent_vertex_array_.Initialize(plane)) return false;
    if (!translucent_texture_.Initialize(translucent_image.width, translucent_image.height,
                                         translucent_image.rgba_pixels.data()))
        return false;
    std::cout << "data is Create!" << std::endl;

    //初始化program
    if (!shader_program_.Initialize(vertex_source.c_str(), fragment_source.c_str())) return false;
    std::cout << "shader_program is Initialized!" << std::endl;
    shader_program_.Use();
    if (!shader_program_.SetVec3("light_direction", glm::normalize(glm::vec3(0.4f, 0.8f, 1.0f)))) {
        return false;
    }
    if (!shader_program_.SetInt("texture_sampler", 0)) {
        return false;
    }
    std::cout << "Renderer is created!" << std::endl;
    return true;
}

bool Renderer::DrawFrame(
    const float elapsed_seconds,
    const int framebuffer_width,
    const int framebuffer_height,
    const glm::mat4 &view
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
    RenderCommand::SetGlobalDepth(true, true, RenderCommand::DepthCompare::LESS);
    RenderCommand::SetGlobalCullFace(true, RenderCommand::CullFace::Back,
                                     RenderCommand::FrontFaceWinding::CounterClockwise);
    RenderCommand::SetBlendingEnabled(false);
    RenderCommand::Clear(0.0, 0.0, 0.0, 1);

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
    if (!shader_program_.SetMat4("model", CubeModel(elapsed_seconds))) {
        return false;
    }
    if (!shader_program_.SetVec4("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))) {
        return false;
    }
    if (!shader_program_.SetInt("use_diffuse_lighting", 1)) {
        return false;
    }
    cube_vertex_array_.Bind();
    cube_texture_.Bind(0);
    RenderCommand::DrawIndexedTriangles(cube_vertex_array_.IndexCount());
    if (!DrawTransparentSurfaces(view)) {
        return false;
    }

#ifndef NDEBUG
    return OpenGLDebug::CheckErrors("Renderer::DrawFrame");
#endif
    return true;
}

bool Renderer::DrawTransparentSurfaces(const glm::mat4 &view) {
    const auto items = CreateTransparentDrawItems(view);
    if (!shader_program_.SetInt("use_diffuse_lighting", 0)) {
        return false;
    }
    RenderCommand::SetDepthWriteEnabled(false);
    RenderCommand::SetBlendingEnabled(true);
    RenderCommand::SetBlendFunction(RenderCommand::BlendFactor::SourceAlpha,
                                    RenderCommand::BlendFactor::OneMinusSourceAlpha);
    RenderCommand::SetFaceCullingEnabled(false);
    translucent_vertex_array_.Bind();
    translucent_texture_.Bind(0);

    bool success = true;
    for (const auto &item: items) {
        if (!shader_program_.SetMat4("model", item.model) ||
            !shader_program_.SetVec4("tint", item.tint)) {
            success = false;
            break;
        }
        RenderCommand::DrawIndexedTriangles(translucent_vertex_array_.IndexCount());
    }
    // Restore the opaque baseline even if a uniform upload fails.
    RenderCommand::SetDepthWriteEnabled(true);
    RenderCommand::SetBlendingEnabled(false);
    RenderCommand::SetFaceCullingEnabled(true);
    return success;
}
