#pragma once

struct GLFWwindow;

class GlfwWindow {
public:
    struct MovementKeyState {
        bool w_pressed = false;
        bool a_pressed = false;
        bool s_pressed = false;
        bool d_pressed = false;
        bool super_camera_  = false;
    };
    GlfwWindow() = default;
    ~GlfwWindow();

    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;
    GlfwWindow(GlfwWindow&&) = delete;
    GlfwWindow& operator=(GlfwWindow&&) = delete;

    struct MouseInputState {
        double delta_x = 0.0;
        double delta_y = 0.0;
        bool left_pressed = false;
        bool right_pressed = false;
    };


    [[nodiscard]] MouseInputState ReadMouseInput();
    void SetCursorCaptured(bool captured);

    bool Initialize(
        int width,
        int height,
        const char* title,
        bool fullscreen_on_secondary_monitor
    );
    bool ShouldClose() const;
    void PollEvents();
    void RequestClose();
    void Present();
    [[nodiscard]] MovementKeyState GetMovementKeyState() const;
    void GetFramebufferSize(int& width, int& height) const;


private:
    static void OnFramebufferSizeChanged(
    GLFWwindow* window,
    int width,
    int height
);
    static void OnKeyChanged(
        GLFWwindow *window, int key, int scancode, int action, int mods);
    double previous_cursor_x_ = 0.0;
    double previous_cursor_y_ = 0.0;
    bool has_cursor_baseline_ = false;
    GLFWwindow* handle_ = nullptr;
    bool glfw_initialized_ = false;
};
