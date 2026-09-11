#version 410 core
out vec4 fragment_color;
in vec2 uv_coord;
in vec3 normal_direction;


uniform sampler2D texture_sampler;
uniform vec4 tint;
uniform int visualize_normals;

void main() {
    vec4 texel = texture(texture_sampler, uv_coord);
    vec3 normal_color = normalize(normal_direction) * 0.5 + 0.5;
    if (visualize_normals != 0){
        fragment_color = vec4(normal_color, 1.0);
    } else {
        fragment_color = tint * texel;
    }
}
