#pragma once
#include "platform/GlfwWindow.h"
#include "render/Renderer.h"

class Application {
public:
    int Run();

private:
    GlfwWindow window_;
    Renderer renderer_;
};
