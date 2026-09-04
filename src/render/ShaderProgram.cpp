#include "ShaderProgram.h"
#include <string>
#include <iostream>
#include <OpenGL/gl3.h>

bool ShaderProgram::Initialize(const char *vertex, const char *fragment) {
    if (program_id_!=0) return false;
    unsigned int vid = CompileShader(GL_VERTEX_SHADER, vertex);
    if (vid == 0) return false;
    unsigned int fid = CompileShader(GL_FRAGMENT_SHADER, fragment);
    if (fid == 0) {
        glDeleteShader(vid);
        return false;
    }
    unsigned int pid = LinkProgram(vid,fid);

    //链接成功删除shader并使用program
    glDeleteShader(vid);
    glDeleteShader(fid);
    if (pid == 0) return false;
    program_id_ = pid;
    return true;

}

void ShaderProgram::Use()  const{
    glUseProgram(program_id_);
}

ShaderProgram::~ShaderProgram() {
    if (program_id_!=0) glDeleteProgram(program_id_);
}


unsigned int ShaderProgram::CompileShader(unsigned int shader_type, const char *source) {
    GLint success = GL_FALSE;
    GLint log_length = 0;
    GLuint id = glCreateShader(shader_type);
    if (!id) {
        std::cout << "Create shader : false"<< std::endl;
        return 0;
    }
    glShaderSource(id, 1, &source, nullptr);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_length);
    std::string info_log(static_cast<std::size_t>(log_length),
    '\0');
    glGetShaderInfoLog(id, log_length, nullptr, info_log.data());
    if (success == GL_FALSE) {
        glDeleteShader(id);
        std::cout << "Compile : " << success << std::endl;
        std::cout << "Log : " << info_log << std::endl;
        return 0;
    }
    return id;
}

unsigned int ShaderProgram::LinkProgram(unsigned int vid, unsigned int fid) {
    GLint success = GL_FALSE;
    GLint log_length = 0;
    //链接program
    auto pid = glCreateProgram();
    if (pid == 0) {
        std::cout << "Create program : false"<< std::endl;
        return 0;
    }
    glAttachShader(pid, vid);
    glAttachShader(pid, fid);
    glLinkProgram(pid);
    glGetProgramiv(pid, GL_LINK_STATUS, &success);
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &log_length);
    std::string info_log(static_cast<std::size_t>(log_length),
    '\0');
    glGetProgramInfoLog(pid, log_length, nullptr, info_log.data());
    std::cout << "Link : " << success << std::endl;
    std::cout << "Log : " << info_log << std::endl;
    if (success == GL_FALSE) {
        glDeleteProgram(pid);
        return 0;
    }
    return pid;
}