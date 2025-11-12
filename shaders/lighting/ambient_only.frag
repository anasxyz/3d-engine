#version 420 core

in vec4 fcolour;
out vec4 outputColor;

uniform float ambientStrength = 0.1;
uniform vec3 lightColour = vec3(1.0, 1.0, 1.0);

void main()
{
	vec3 ambient = ambientStrength * lightColour;
	outputColor = vec4(ambient, 1.0);
}
