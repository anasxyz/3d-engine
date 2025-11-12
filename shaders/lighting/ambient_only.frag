#version 420 core

in vec4 fragColour;
out vec4 outputColour;

uniform float ambientStrength = 0.1;
uniform vec3 lightColour = vec3(1.0, 1.0, 1.0);

void main() {
	vec3 ambient = ambientStrength * lightColour;
	outputColour = vec4(ambient, 1.0);
}
