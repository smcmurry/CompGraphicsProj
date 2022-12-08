#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space

// varying outputs
out vec3 TexCoord;

void main() {
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    TexCoord = vPos;
}
