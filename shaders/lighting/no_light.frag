#version 420 core

in vec4 fragColour;
out vec4 outputColour;

void main() {
	outputColour = vec4(fragColour.rgb, 1.0);
}
