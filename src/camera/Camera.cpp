//
// Created by 陈泓池 on 2026/9/9.
//

#include "Camera.h"

#include <glm/ext/matrix_transform.hpp>

void Camera::SetPosition(const glm::vec3& position) {
    position_ = position;
}

glm::mat4 Camera::ViewMatrix() const {
    return glm::lookAt(position_, position_ + forward_, up_);
}

void Camera::Move(const glm::vec3& displacement) {
    position_ += displacement;
}
