#pragma once

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
    void DrawFrame();

private:
    ShaderProgram shader_program_;
    VertexArray vertex_array_;
    Texture2D texture_;
};
