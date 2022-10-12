#version 410 core

// uniform inputs
uniform vec3 lightDirection;
uniform vec3 lightColor;


uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// varying inputs
layout(location = 0) in vec3 normal;     // interpolated color for this fragment

// outputs
out vec4 fragColorOut;                  // color to apply to this fragment

void main() {

    // TODO #B: computer Light vector
    vec3 L = -normalize(lightDirection);
    // pass the interpolated color through as output
    vec3 diffuse = lightColor*materialColor*max(dot(normal, L), 0.0);
    fragColorOut = vec4(0.25*diffuse+0.1*materialColor, 1.0);
}