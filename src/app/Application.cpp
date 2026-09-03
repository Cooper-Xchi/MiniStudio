#include "Application.h"
#include "platform/GlfwWindow.h"

int Application::Run() {
    if (!window_.Initialize(1280,960,"MiniStudio"))return 1;
    while (!window_.ShouldClose()) {
        window_.PollEvents();
        if (window_.IsEscapePressed()) {
            window_.RequestClose();
        }
    }
    return 0;
}
