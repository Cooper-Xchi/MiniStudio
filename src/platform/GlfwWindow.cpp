#include "GlfwWindow.h"
#include "opengl/OpenGLHeaders.h"
#include <iostream>
#include <GLFW/glfw3.h>


GlfwWindow::~GlfwWindow() {
    if (handle_ != nullptr) {
        glfwDestroyWindow(handle_);
    }
    if (glfw_initialized_)
        glfwTerminate();
}

bool GlfwWindow::Initialize(
    int width,
    int height,
    const char* title,
    bool fullscreen_on_secondary_monitor
) {
    //glfw初始化
    if (glfwInit() == GLFW_FALSE) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        glfw_initialized_ = false;
        return false;
    }
    glfw_initialized_ = true;
    std::cout << "GLFW initialized!" << std::endl;
    //OpenGl——api版本初始化
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    //window初始化
    GLFWmonitor* target_monitor = nullptr;
    int actual_width = width;
    int actual_height = height;

    if (fullscreen_on_secondary_monitor) {
        int monitor_count = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);

        if (monitors == nullptr || monitor_count < 2) {
            std::cerr
                << "Secondary monitor unavailable; falling back to windowed mode."
                << std::endl;
        } else {
            target_monitor = monitors[1];
            const GLFWvidmode* mode = glfwGetVideoMode(target_monitor);

            if (mode == nullptr) {
                std::cerr
                    << "Secondary monitor video mode unavailable; falling back to windowed mode."
                    << std::endl;
                target_monitor = nullptr;
            } else {
                actual_width = mode->width;
                actual_height = mode->height;
            }
        }
    }

    GLFWwindow* window = glfwCreateWindow(
        actual_width,
        actual_height,
        title,
        target_monitor,
        nullptr
    );
    if (window == nullptr) {
        std::cerr << "GLFW Window creation failed!" << std::endl;
        return false;
    }
    handle_ = window;
    glfwMakeContextCurrent(window);
#if defined(_WIN32)
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return false;
    }
#endif
    glfwSetFramebufferSizeCallback(
        handle_,
        OnFramebufferSizeChanged
    );
    int major = glfwGetWindowAttrib(window,GLFW_CONTEXT_VERSION_MAJOR);
    int minor = glfwGetWindowAttrib(window,GLFW_CONTEXT_VERSION_MINOR);
    int profile = glfwGetWindowAttrib(window, GLFW_OPENGL_PROFILE);
    bool is_core_profile = profile == GLFW_OPENGL_CORE_PROFILE;
    std::cout << "GLFW context version " << major << "." << minor << std::endl;
    std::cout << "GLFW core profile equal profile： " << is_core_profile << std::endl;
    std::cout << "GLFW Window created!" << std::endl;
    int f_width;
    int f_height;
    glfwGetWindowSize(window,&width,&height);
    glfwGetFramebufferSize(window,&f_width,&f_height);
    std::cout<<"windowSize: "<<width<<"x"<<height<<std::endl;
    std::cout<<"FrameBufferSize"<<f_width<<"x"<<f_height<<std::endl;
    glViewport(0,0,f_width,f_height);

    return true;
}


void GlfwWindow::OnFramebufferSizeChanged(GLFWwindow *,int width,int height) {
    glViewport(0,0,width,height);
}

bool GlfwWindow::ShouldClose() const {
    return glfwWindowShouldClose(handle_) == GLFW_TRUE;
}

void GlfwWindow::PollEvents() {
    glfwPollEvents();
}

bool GlfwWindow::IsEscapePressed() const {
    return glfwGetKey(handle_, GLFW_KEY_ESCAPE) == GLFW_PRESS;
}

void GlfwWindow::RequestClose() {
    glfwSetWindowShouldClose(handle_, GLFW_TRUE);
}

void GlfwWindow::Present() {
    glfwSwapBuffers(handle_);
}

void GlfwWindow::GetFramebufferSize(int& width, int& height) const {
    glfwGetFramebufferSize(handle_, &width, &height);
}

GlfwWindow::MovementKeyState GlfwWindow::GetMovementKeyState() const {
    MovementKeyState movement_key_state;
    if (glfwGetKey(handle_, GLFW_KEY_W) == GLFW_PRESS) {
        movement_key_state.w_pressed = true;
    }
    if (glfwGetKey(handle_, GLFW_KEY_S) == GLFW_PRESS) {
        movement_key_state.s_pressed = true;
    }
    if (glfwGetKey(handle_, GLFW_KEY_A) == GLFW_PRESS) {
        movement_key_state.a_pressed = true;
    }
    if (glfwGetKey(handle_, GLFW_KEY_D) == GLFW_PRESS) {
        movement_key_state.d_pressed = true;
    }
    return movement_key_state;
}

GlfwWindow::MouseInputState GlfwWindow::ReadMouseInput() {
    MouseInputState mouse_state;
    if (glfwGetWindowAttrib(handle_, GLFW_FOCUSED) == GLFW_FALSE) {
        has_cursor_baseline_ = false;
        return mouse_state;
    }
    if (glfwGetMouseButton(handle_,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        mouse_state.left_pressed = true;
    }
    if (glfwGetMouseButton(handle_,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        mouse_state.right_pressed = true;
    }
    if (glfwGetInputMode(handle_, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
        has_cursor_baseline_ = false;
        return mouse_state;
    }
    if (!has_cursor_baseline_ ) {
        double cursor_pos_x;
        double cursor_pos_y;
        glfwGetCursorPos(handle_,&cursor_pos_x,&cursor_pos_y);
        previous_cursor_x_ = cursor_pos_x;
        previous_cursor_y_ = cursor_pos_y;
        mouse_state.delta_x = 0.0f;
        mouse_state.delta_y = 0.0f;
        has_cursor_baseline_ = true;
        return mouse_state;
    }
    double current_cursor_x_;
    double current_cursor_y_;
    glfwGetCursorPos(handle_,&current_cursor_x_,&current_cursor_y_);
    mouse_state.delta_x = current_cursor_x_ - previous_cursor_x_;
    mouse_state.delta_y = current_cursor_y_ - previous_cursor_y_;
    previous_cursor_x_ = current_cursor_x_;
    previous_cursor_y_ = current_cursor_y_;


    return mouse_state;
}
void GlfwWindow::SetCursorCaptured(bool captured) {
    if (handle_ == nullptr) {
        return ;
    }
    const int target_mode = captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
    const int current_mode = glfwGetInputMode(handle_,GLFW_CURSOR);
    if (target_mode == current_mode) {
        return;
    }else {
        glfwSetInputMode(handle_,GLFW_CURSOR,captured?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);
        has_cursor_baseline_ = false;
    }
}
