#version 330 core

layout(location = 0) in vec3 posAttr;
layout(location = 1) in vec3 normAttr;
layout(location = 2) in vec2 uvAttr;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 uv;
} vs_out;

void main() {
    vs_out.fragPos = posAttr;
    vs_out.normal = normAttr;
    vs_out.uv = uvAttr;

    gl_Position = vec4(vs_out.fragPos, 1);
}
