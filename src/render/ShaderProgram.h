#pragma once
#include "glm/glm.hpp"


class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept ;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    bool Initialize(
        const char* vertex,
        const char* fragment
        );

    void Use() const;

    bool SetInt(const char* name, int value);

    bool SetMat4(const char* name, const glm::mat4& value);





private:

    void Release();
    static unsigned int CompileShader(
unsigned int shader_type,
const char* source
);

    static unsigned int LinkProgram(
    unsigned int vid, unsigned int fid
);
    unsigned int program_id_ = 0;
};
