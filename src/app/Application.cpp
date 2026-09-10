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
    constexpr float camera_speed = 0.25f;
    constexpr float mouse_sensitivity = 0.1f;
    const auto start_time = std::chrono::steady_clock::now();
    auto previous_time = start_time;
    camera_.SetPosition(glm::vec3(0.25f, 0.0f, 0.0f));
    while (!window_.ShouldClose()) {
        window_.PollEvents();
        if (window_.IsEscapePressed()) {
            window_.RequestClose();
        }
        const auto current_time = std::chrono::steady_clock::now();
        const float elapsed_seconds = std::chrono::duration<float>(current_time - start_time).count();
        const float delta_seconds = std::chrono::duration<float>(current_time - previous_time).count();
        previous_time = current_time;
        glm::vec3 movement{0.0f};
        const GlfwWindow::MovementKeyState keys = window_.GetMovementKeyState();
        const GlfwWindow::MouseInputState mouse = window_.ReadMouseInput();
        if (keys.w_pressed) movement.z -= 1.0f;
        if (keys.s_pressed) movement.z += 1.0f;
        if (keys.a_pressed) movement.x -= 1.0f;
        if (keys.d_pressed) movement.x += 1.0f;
        if (mouse.left_pressed) {
            window_.SetCursorCaptured(true);
        }
        if (mouse.right_pressed) {
            window_.SetCursorCaptured(false);
        }
        const float yaw_delta =
            static_cast<float>(mouse.delta_x) * mouse_sensitivity;
        const float pitch_delta =
            static_cast<float>(-mouse.delta_y) * mouse_sensitivity;
        camera_.Move(movement * camera_speed * delta_seconds);
        camera_.Rotate(yaw_delta, pitch_delta);
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
