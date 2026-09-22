#pragma once

#include <glm/mat4x4.hpp>

#include "Material.h"
#include "Texture2D.h"
#include "render/ShaderProgram.h"
#include "render/VertexArray.h"

class Renderer {
public:
    Renderer() = default;

    Renderer(const Renderer &) = delete;

    Renderer &operator=(const Renderer &) = delete;

    Renderer(Renderer &&) noexcept = default;

    Renderer &operator=(Renderer &&) noexcept = default;

    bool Initialize();

    bool DrawFrame(
        float elapsed_seconds,
        int framebuffer_width,
        int framebuffer_height,
        const glm::mat4 &view,
        const glm::vec4 &camera_position
    );

private:
    bool DrawTransparentSurfaces(const glm::mat4 &view);

    bool SetMaterialUniform(const Material &mat);

    bool SetModelUniforms(const glm::mat4 &model);

    ShaderProgram shader_program_;
    VertexArray cube_vertex_array_;
    Texture2D cube_texture_;

    VertexArray plane_vertex_array_;
    Texture2D translucent_texture_;
};
