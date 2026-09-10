#include "Camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <cmath>

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
    pitch_degrees_ = glm::clamp(pitch_degrees_, -89.0f, 89.0f);
    UpdateForward();



}

glm::vec3 Camera::Forward() const {
    return forward_;


}


void Camera::UpdateForward() {
    float yaw_radians = glm::radians(yaw_degrees_);
    float pitch_radians = glm::radians(pitch_degrees_);
    auto x = std::cos(yaw_radians) * std::cos(pitch_radians);
    auto y = std::sin(pitch_radians);
    auto z = std::sin(yaw_radians) * std::cos(pitch_radians);
    forward_ = glm::normalize(glm::vec3(x, y, z));
}
