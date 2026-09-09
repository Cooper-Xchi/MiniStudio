#pragma once

struct GLFWwindow;

class GlfwWindow {
public:
    struct MovementKeyState {
        bool w_pressed = false;
        bool a_pressed = false;
        bool s_pressed = false;
        bool d_pressed = false;
    };
    GlfwWindow() = default;
    ~GlfwWindow();

    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;
    GlfwWindow(GlfwWindow&&) = delete;
    GlfwWindow& operator=(GlfwWindow&&) = delete;

    bool Initialize(
        int width,
        int height,
        const char* title,
        bool fullscreen_on_secondary_monitor
    );
    bool ShouldClose() const;
    void PollEvents();
    bool IsEscapePressed() const;
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

    GLFWwindow* handle_ = nullptr;
    bool glfw_initialized_ = false;
};
