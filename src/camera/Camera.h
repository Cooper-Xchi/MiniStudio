#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Camera {
public:
    void SetPosition(const glm::vec3& position);
    [[nodiscard]] glm::mat4 ViewMatrix() const;
    void Move(const glm::vec3& displacement);

private:
    glm::vec3 position_{0.0f};
    glm::vec3 forward_{0.0f, 0.0f, -1.0f};
    glm::vec3 up_{0.0f, 1.0f, 0.0f};
};
