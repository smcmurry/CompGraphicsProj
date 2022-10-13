#version 410 core

#define MAX_LIGHTS 10
// uniform inputs
uniform mat4 mMatrix;                 // the precomputed Model Matrix
uniform mat4 vMatrix;                 // the precomputed View Matrix
uniform mat4 pMatrix;                 // the precomputed Projection Matrix
uniform mat3 normalMatrix;
uniform vec3 materialColor;             // the material color for our vertex (& whole object)
uniform vec3 ambientColor;
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightDirections[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightSizes[MAX_LIGHTS];
uniform uint lightTypes[MAX_LIGHTS];
uniform uint lightCount;

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
layout(location = 1) in vec3 vNormal;


// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex

void main() {
    // transform & output the vertex in clip space
    vec4 worldSpace = mMatrix * vec4(vPos, 1.0);
    vec4 viewSpace = vMatrix * worldSpace;
    gl_Position = pMatrix * viewSpace;

    vec3 normal = normalMatrix * vNormal;

    color = vec3(0);
    for(int i = 0; i < lightCount; i++) {
        vec3 L;
        if(lightTypes[i] == 0) {
            L = normalize(lightDirections[i]);
        } else {
            L = normalize(vec3(worldSpace) - lightPositions[i]);
        }
        vec3 lightColor = lightColors[i]*materialColor*max(dot(normal, -L), 0.0);
        if(lightTypes[i] != 0) {
            lightColor /= pow(length(vec3(worldSpace) - lightPositions[i]), 2);
        }
        color += lightColor;
    }
    color += ambientColor*materialColor;
}