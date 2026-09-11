#pragma once
#include <cstdint>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


class MeshData {
public:
    struct Vertex {
        glm::vec3 position{};
        glm::vec3 color{};
        glm::vec2 uv{};
        glm::vec3 normal{};
    };

    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
};
