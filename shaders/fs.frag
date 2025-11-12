#version 420 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec4 fragColour;

out vec4 outputColour;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 lightColour;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

void main() {
  vec3 objectColour = vec3(fragColour.rgb);

  vec3 N = normalize(fragNormal);
  vec3 L = normalize(lightPosition - fragPosition);
  vec3 V = normalize(viewPosition - fragPosition);
  vec3 R = reflect(-L, N);

 // ambient
  vec3 ambient = ambientStrength * lightColour;

 // diffuse 
  float diff = max(dot(N, L), 0.0);
  vec3 diffuse = diff * lightColour;

	// specular
  float spec = pow(max(dot(V, R), 0.0), shininess);
  vec3 specular = spec * lightColour;

	// all together
  vec3 result = (ambient + diffuse + specular) * objectColour;
  outputColour = vec4(result, 1.0);
}
