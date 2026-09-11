#include "VertexArray.h"
#include "opengl/OpenGLHeaders.h"
#include <utility>
#include <cstddef>

bool VertexArray::Initialize(const MeshData& mesh) {
    for (const std::uint32_t index : mesh.indices) {
        if (index >= mesh.vertices.size()) {
            return false;
        }
    }
    if (mesh.vertices.empty()
        || mesh.indices.empty()
        || vao_ != 0
        || vbo_ != 0
        || ebo_ != 0) return false;
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
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(mesh.vertices.size() * sizeof(MeshData::Vertex)),mesh.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshData::Vertex), reinterpret_cast<const void*>(offsetof(MeshData::Vertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshData::Vertex), reinterpret_cast<const void*>(offsetof(MeshData::Vertex, color)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshData::Vertex), reinterpret_cast<const void*>(offsetof(MeshData::Vertex, uv)));
    glEnableVertexAttribArray(2);
    glGenBuffers(1, &ebo_);
    if (!ebo_) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
        glDeleteBuffers(1,&vbo_);
        vbo_ = 0;
        return false;
    }
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,static_cast<GLsizeiptr>(mesh.indices.size() * sizeof(uint32_t)),mesh.indices.data(), GL_STATIC_DRAW);
    index_count_ = static_cast<int>(mesh.indices.size());
    vertex_count_ = static_cast<int>(mesh.vertices.size());
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

int VertexArray::IndexCount() const {
    return index_count_;
}

VertexArray::~VertexArray() {
    Release();
}

void VertexArray::Release() {
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    index_count_ = 0;
    vertex_count_ = 0;
}

VertexArray::VertexArray(VertexArray&& other) noexcept:
vao_(other.vao_),
vbo_(other.vbo_),
ebo_(other.ebo_),
index_count_(other.index_count_),
vertex_count_(other.vertex_count_){
    other.vao_ = 0;
    other.vbo_ = 0;
    other.ebo_ = 0;
    other.index_count_ = 0;
    other.vertex_count_ = 0;
}
VertexArray& VertexArray::operator=(VertexArray&& other) noexcept{
    if (this == &other) return *this;
    Release();
    vao_ = std::exchange(other.vao_,0);
    vbo_ = std::exchange(other.vbo_,0);
    ebo_ = std::exchange(other.ebo_,0);
    index_count_ = std::exchange(other.index_count_,0);
    vertex_count_ = std::exchange(other.vertex_count_,0);
    return *this;

}
