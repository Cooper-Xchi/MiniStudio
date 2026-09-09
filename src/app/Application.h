#pragma once
#include "camera/Camera.h"
#include "platform/GlfwWindow.h"
#include "render/Renderer.h"

class Application {
public:
    int Run();

private:
    GlfwWindow window_;
    Renderer renderer_;
    Camera camera_;
};
