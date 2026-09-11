#version 410 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;
out vec2 uv_coord;
out vec3 world_normal;

void main() {
    uv_coord = uv;
    world_normal = normal_matrix * normal;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
