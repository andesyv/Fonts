#version 430 core

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 uv;
} fs_in;

out vec4 fragColor;

void main() {
    fragColor = vec4(vec3(fs_in.uv, 0), 1);
}
