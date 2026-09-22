#pragma once
#include <glm/vec3.hpp>


struct Material {
    glm::vec3 diffuse_color; // 漫反射基色
    glm::vec3 specular_color; // 高光颜色
    float shininess; // 高光锐度指数
};
