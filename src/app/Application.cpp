#include "Application.h"
#include <chrono>

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
    auto start_time = std::chrono::steady_clock::now();
    camera_.SetPosition(glm::vec3(0.25f,0.0f,0.0f));
    while (!window_.ShouldClose()) {
        window_.PollEvents();
        if (window_.IsEscapePressed()) {
            window_.RequestClose();
        }
        auto current_time = std::chrono::steady_clock::now();
        const float elapsed_seconds = std::chrono::duration<float>(current_time - start_time ).count();
        int framebuffer_width = 0;
        int framebuffer_height = 0;
        window_.GetFramebufferSize(framebuffer_width, framebuffer_height);
        const glm::mat4 view = camera_.ViewMatrix();
        if (!renderer_.DrawFrame(
                elapsed_seconds,
                framebuffer_width,
                framebuffer_height,
                view
            )) {
            return 1;
        }
        window_.Present();

    }
    return 0;
}
