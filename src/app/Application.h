#pragma once
#include "platform/GlfwWindow.h"

class Application {
public:
    int Run();

private:
    GlfwWindow window_;
};
