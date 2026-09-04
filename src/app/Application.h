#pragma once
#include "platform/GlfwWindow.h"
#include "render/ShaderProgram.h"

class Application {
public:
    int Run();

private:
    GlfwWindow window_;
    ShaderProgram shader_program_;
};
