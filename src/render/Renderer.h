#pragma once

#include "render/ShaderProgram.h"
#include "render/VertexArray.h"

class Renderer {
public:
    bool Initialize();
    void DrawFrame();

private:
    ShaderProgram shader_program_;
    VertexArray vertex_array_;
};
