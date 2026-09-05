#include "OpenGLDebug.h"
#include "opengl/OpenGLHeaders.h"

#include <iostream>



void OpenGLDebug::ClearErrors() {
    while (glGetError() != GL_NO_ERROR) {
        // 不需要处理，只负责取出旧错误
    }
}


bool OpenGLDebug::CheckErrors(const char* label){
    bool result = true;
    while (true) {
        const unsigned int error = glGetError();
        if (error == GL_NO_ERROR) {
            break;
        }
        result = false;
        std::cerr
    << label
    << " : 0x"
    << std::hex
    << error
    << std::dec
    << '\n';
    }

    return result;


}
