#pragma once

#include <glm/mat4x4.hpp>
#include "Texture2D.h"
#include "render/ShaderProgram.h"
#include "render/VertexArray.h"

class Renderer {
public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) noexcept = default;
    Renderer& operator=(Renderer&&) noexcept = default;

    bool Initialize();
    bool DrawFrame(
        float elapsed_seconds,
        int framebuffer_width,
        int framebuffer_height,
        const glm::mat4& view
    );

private:
    ShaderProgram shader_program_;
    VertexArray cube_vertex_array_;
    Texture2D cube_texture_;

    VertexArray translucent_vertex_array_;
    Texture2D translucent_texture_;
};
