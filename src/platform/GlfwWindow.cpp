#include "GlfwWindow.h"
#include <string>
#include <iostream>
#include <GLFW/glfw3.h>


GlfwWindow::~GlfwWindow() {
    if (handle_ != nullptr) {
        glfwDestroyWindow(handle_);
    }
    if (glfw_initialized_)
        glfwTerminate();
}

bool GlfwWindow::Initialize(int width, int height, const char* title) {
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
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "GLFW Window creation failed!" << std::endl;
        return false;
    }
    handle_ = window;
    glfwSetFramebufferSizeCallback(
    handle_,
    OnFramebufferSizeChanged
);
    glfwMakeContextCurrent(window);
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
    std::cout<<width<<"x"<<height<<std::endl;
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
