//
// Created by 陈泓池 on 2026/9/9.
//

#include "Camera.h"

#include <iostream>
#include <ostream>
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

void Camera::Rotate(
    float yaw_delta_degrees,
    float pitch_delta_degrees
) {
    yaw_degrees_ += yaw_delta_degrees;
    pitch_degrees_ += pitch_delta_degrees;
    if (pitch_degrees_ >= 90.0f) pitch_degrees_ = 90.0f;
    else if (pitch_degrees_ <= -90.0f) pitch_degrees_ = -90.0f;
    UpdateForward();



}

glm::vec3 Camera::Forward() const {



}


void Camera::UpdateForward() {
    float yaw_radians = glm::radians(yaw_degrees_);
    float pitch_radians = glm::radians(pitch_degrees_);
    auto x = cos(yaw_radians) * cos(pitch_radians);
    auto y = sin(pitch_radians);
    auto z = sin(yaw_radians) * cos(pitch_radians);
    forward_ = glm::vec3(x, y, z);
}
