#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
// TODO #D: add normal matrix
uniform mat3 normalMatrix;

// TODO #A: add light uniforms

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
// TODO #C: add vertex normal
layout(location = 1) in vec3 vNormal;


// varying outputs
layout(location = 0) out vec3 normal;    // color to apply to this vertex

void main() {
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // TODO #E: transform normal vector
    normal = normalMatrix * vNormal;
}