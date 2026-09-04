#pragma once


class ShaderProgram {
public:
    ShaderProgram() = default;
    ~ShaderProgram();
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram(ShaderProgram&&) = delete;
    ShaderProgram& operator=(ShaderProgram&&) = delete;

    bool Initialize(
        const char* vertex,
        const char* fragment
        );

    void Use() const;



private:
    static unsigned int CompileShader(
unsigned int shader_type,
const char* source
);

    static unsigned int LinkProgram(
    unsigned int vid, unsigned int fid
);
    unsigned int program_id_ = 0;
};
