#include "VertexArray.h"
#include <OpenGL/gl3.h>

bool VertexArray::Initialize(const float* data, std::size_t float_count) {
    if (data == nullptr
        || float_count  ==0
        || float_count % 6 !=0
        || vao_ != 0
        || vbo_ != 0) return false;
    glGenVertexArrays(1, &vao_);
    if (!vao_) return false;
    glGenBuffers(1, &vbo_);
    if (!vbo_) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
        return false;
    }
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(float_count * sizeof(float)),data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), reinterpret_cast<const void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    vertex_count_ = static_cast<int>(float_count/6);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}
void VertexArray::Bind() const {
    glBindVertexArray(vao_);
}

int VertexArray::VertexCount() const {
    return vertex_count_;

}

VertexArray::~VertexArray() {
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
}
