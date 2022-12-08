#version 410 core

// uniform inputs
uniform samplerCube texMap;

// varying inputs
in vec3 TexCoord;

// fragment outputs
out vec4 fragColorOut;

void main() {
    vec4 texel = texture(texMap, TexCoord);

    fragColorOut = texel;
}
