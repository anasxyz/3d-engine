#version 420 core

in vec3 fragNormal;
in vec3 fragPosition;

out vec4 outputColour;

uniform vec3 lightPosition;
uniform vec3 lightColour = vec3(1.0, 1.0, 1.0);

void main() {
    vec3 N = normalize(fragNormal);
    vec3 L = normalize(lightPosition - fragPosition);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColour;
    outputColour = vec4(diffuse, 1.0);
}
