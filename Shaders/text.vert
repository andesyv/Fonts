#version 430 core

layout(location = 0) in vec4 posAndUVAttr;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

out vec2 TexCoords;

void main() {

    TexCoords = posAndUVAttr.zw;
    gl_Position = vec4(posAndUVAttr.xy, 0, 1);
}
