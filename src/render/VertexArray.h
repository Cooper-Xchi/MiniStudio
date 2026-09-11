#pragma once
#include "mesh/MeshData.h"

class VertexArray {
public:
    VertexArray() = default;
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    bool Initialize(const MeshData& mesh);
    void Bind() const;
    int VertexCount() const;
    int IndexCount() const;
private:
    void Release();
    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;
    int index_count_ = 0;
    int vertex_count_ = 0;
};
