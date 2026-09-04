#pragma once
#include <cstddef>

class VertexArray {
public:
    VertexArray() = default;
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&&) = delete;
    VertexArray& operator=(VertexArray&&) = delete;

    bool Initialize(const float* data, std::size_t float_count);
    void Bind() const;
    int VertexCount() const;

private:
    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    int vertex_count_ = 0;
};