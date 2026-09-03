#include <iostream>
#include <GLFW/glfw3.h>

bool Init() {
    int result = glfwInit();
    if (result == GLFW_FALSE) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return false;
    }
    std::cout << "GLFW initialized!" << std::endl;
    return true;
}

void SetContextHints() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
}

int Render() {
    GLFWwindow* window = glfwCreateWindow(800, 600, "MiniStudio", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "GLFW Window creation failed!" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    int major = glfwGetWindowAttrib(window,GLFW_CONTEXT_VERSION_MAJOR);
    int minor = glfwGetWindowAttrib(window,GLFW_CONTEXT_VERSION_MINOR);
    int profile = glfwGetWindowAttrib(window, GLFW_OPENGL_PROFILE);
    bool is_core_profile = profile == GLFW_OPENGL_CORE_PROFILE;
    std::cout << "GLFW context version " << major << "." << minor << std::endl;
    std::cout << "GLFW core profile equal profile? " << is_core_profile << std::endl;
    std::cout << "GLFW Window created!" << std::endl;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

int main() {
    if (!Init()) return 1;
    SetContextHints();
    return Render();
}
