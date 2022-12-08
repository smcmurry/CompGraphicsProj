#version 410 core

uniform sampler2D texMap;
uniform int useTexture;

// varying inputs
layout(location = 0) in vec3 color;     // interpolated color for this fragment
layout(location = 1) in vec2 TexCoordOut;

// outputs
out vec4 fragColorOut;                  // color to apply to this fragment

void main() {
    if (useTexture == 1)
    {
        vec4 texel = texture(texMap, TexCoordOut);
        fragColorOut = vec4(color, 1.0) + texel;
    }
    else
    {
        fragColorOut = vec4(color, 1.0);
    }
}