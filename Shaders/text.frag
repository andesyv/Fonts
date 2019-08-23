#version 430 core

in vec2 TexCoords;

uniform vec2 uvPos = vec2(0, 0);
uniform vec2 uvScale = vec2(1, 1);
uniform sampler2D letter;
uniform vec3 textColor = vec3(1, 1, 1);

out vec4 fragColor;

void main() {
    float alpha = texture(letter, TexCoords).r;
    // if (alpha < 0.0)
    //     discard;
    fragColor = vec4(vec3(1), alpha);
    // fragColor = vec4(TexCoords, 0, 1);
}
