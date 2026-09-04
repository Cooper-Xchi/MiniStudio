#pragma once
#include "render/VertexArray.h"
#include "platform/GlfwWindow.h"
#include "render/ShaderProgram.h"

class Application {
public:
    int Run();

private:
    GlfwWindow window_;
    ShaderProgram shader_program_;
    VertexArray vertex_array_;
};
