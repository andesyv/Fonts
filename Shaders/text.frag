#version 430 core

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 uv;
} fs_in;

uniform vec2 uvPos = vec2(0, 0);
uniform vec2 uvScale = vec2(1, 1);
uniform sampler2D letter;
uniform vec3 textColor = vec3(1, 1, 1);

out vec4 fragColor;

void main() {
    float alpha = texture(letter, fs_in.uv).r;
    fragColor = vec4(textColor, alpha);
}
