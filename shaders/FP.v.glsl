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
uniform float lightSizes[MAX_LIGHTS]; // the angle of the spotlight
uniform uint lightTypes[MAX_LIGHTS]; // light types 0 = directional, 1 = point, 2 = spot
uniform uint lightCount;

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 texCoord;


// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex
layout(location = 1) out vec2 texCoordOut;

void main() {
    // transform & output the vertex in clip space
    vec4 worldSpace = mMatrix * vec4(vPos, 1.0);
    vec4 viewSpace = vMatrix * worldSpace;
    gl_Position = pMatrix * viewSpace;
    texCoordOut = texCoord;

    vec3 normal = normalMatrix * vNormal;

    color = vec3(0);
    for(int i = 0; i < lightCount; i++) {
        vec3 L;
        vec3 R;
        if(lightTypes[i] == 0) {
            L = normalize(lightDirections[i]);
        } else {
            L = normalize(vec3(worldSpace) - lightPositions[i]);
        }
        R = L-2*normal*dot(normal, L);
        float specular = max(0.0, dot(R, -normalize(vec3(viewSpace))));
        specular = pow(specular, 3.3);
        float diffuse = max(0.0, dot(normal, -L));
        vec3 lightColor = lightColors[i]*materialColor*(0.1*diffuse + 0.005*specular);
        if(lightTypes[i] != 0) { // attenuation
            float dist = length(vec3(worldSpace) - lightPositions[i]);
            lightColor /= pow(dist, 2) + 0.5*dist + 1;
        }
        if(lightTypes[i] == 2 && acos(dot(normalize(lightDirections[i]), L)) > lightSizes[i]) { // limit spotlight
            lightColor = vec3(0);
        }
        color += lightColor;
    }
    color += ambientColor*materialColor;
}