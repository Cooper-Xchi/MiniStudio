#include "Application.h"

int Application::Run() {
    constexpr bool fullscreen_on_secondary_monitor = false;
    if (!window_.Initialize(
            1280,
            960,
            "MiniStudio",
            fullscreen_on_secondary_monitor
        )) {
        return 1;
    }
    if (!renderer_.Initialize()) return 1;
    while (!window_.ShouldClose()) {
        window_.PollEvents();
        if (window_.IsEscapePressed()) {
            window_.RequestClose();
        }
        renderer_.DrawFrame();
        window_.Present();

    }
    return 0;
}
