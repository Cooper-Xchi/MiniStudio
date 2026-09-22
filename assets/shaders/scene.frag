#version 410 core
out vec4 fragment_color;
in vec2 uv_coord;
in vec3 world_normal;
in vec4 world_position;
uniform sampler2D texture_sampler;
uniform vec4 camera_position;
uniform vec4 tint;
uniform vec3 light_direction;
uniform int use_diffuse_lighting;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;

void main() {
    vec4 texel = texture(texture_sampler, uv_coord);
    vec3 n = normalize(world_normal);
    vec3 l = normalize(light_direction);
    vec3 v = normalize(camera_position.xyz - world_position.xyz);
    float specular = pow(max(dot((normalize(l + v)), n), 0.0), shininess);
    float diffuse = max(dot(n, l), 0.0);
    if (use_diffuse_lighting != 0) {
        fragment_color = vec4(diffuse * diffuse_color + specular * specular_color, 1.0);
    } else {
        fragment_color = tint * texel;
    }
}
