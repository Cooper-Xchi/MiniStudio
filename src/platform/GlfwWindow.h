#pragma once

struct GLFWwindow;

class GlfwWindow {
public:
    GlfwWindow() = default;
    ~GlfwWindow();

    GlfwWindow(const GlfwWindow&) = delete;
    GlfwWindow& operator=(const GlfwWindow&) = delete;
    GlfwWindow(GlfwWindow&&) = delete;
    GlfwWindow& operator=(GlfwWindow&&) = delete;

    bool Initialize(int width, int height, const char* title);
    bool ShouldClose() const;
    void PollEvents();
    bool IsEscapePressed() const;
    void RequestClose();


private:
    static void OnFramebufferSizeChanged(
    GLFWwindow* window,
    int width,
    int height
);

    GLFWwindow* handle_ = nullptr;
    bool glfw_initialized_ = false;
};
