#include <iostream>
#include <GLFW/glfw3.h>

int run() {
    int result = glfwInit();
    if (result == GLFW_FALSE) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return 1;
    }
    std::cout << "GLFW initialized!" << std::endl;
    GLFWwindow* window = glfwCreateWindow(800, 600, "MiniStudio", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "GLFW Window creation failed!" << std::endl;
        glfwTerminate();
        return 1;
    }
    std::cout << "GLFW Window created!" << std::endl;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

int main() {
    return run();
}
