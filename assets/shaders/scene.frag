#version 410 core
out vec4 fragment_color;
in vec2 uv_coord;
in vec3 world_normal;
uniform sampler2D texture_sampler;
uniform vec4 tint;
uniform vec3 light_direction;
uniform int use_diffuse_lighting;

void main() {
    vec4 texel = texture(texture_sampler, uv_coord);
    vec3 n = normalize(world_normal);
    vec3 l = normalize(light_direction);
    float diffuse = max(dot(n, l), 0.0);
    if (use_diffuse_lighting != 0) {
        fragment_color = vec4(vec3(diffuse), 1.0);
    } else {
        fragment_color = tint * texel;
    }
}
