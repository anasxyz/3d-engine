#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec3 normal;

out vec4 fragColour;
out vec3 fragNormal;
out vec3 fragPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * position;

	fragNormal = mat3(transpose(inverse(model))) * normal;
	fragPosition = vec3(model * position);

	fragColour = colour;
}
