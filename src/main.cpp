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

GLFWwindow* CreateWindow(int width,int height) {
    GLFWwindow* window = glfwCreateWindow(width, height, "MiniStudio", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "GLFW Window creation failed!" << std::endl;
        glfwTerminate();
        return nullptr;
    }
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
    return window;

}

void onSizeChanged(GLFWwindow*,int w,int h) {
    glViewport(0,0,w,h);
    std::cout<<w<<"x"<<h<<std::endl;
}

int Render(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window,onSizeChanged);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window,GLFW_TRUE);
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

int main() {
    int width = 800;
    int height = 600;
    if (!Init()) return 1;
    SetContextHints();
    GLFWwindow* window = CreateWindow(width,height);
    if (window == nullptr) return 1;
    return Render(window);
}
