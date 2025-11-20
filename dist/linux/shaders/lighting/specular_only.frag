// i know it wont show anything on it's own because it needs to be combined with the other stuff
// but i thought i'd do the same as i did with the other ones and have it in it's own file
// because this is where i made it

#version 420 core

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 outputColour;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 lightColour;
uniform float shininess;

void main() {
  vec3 N = normalize(fragNormal);
  vec3 L = normalize(lightPosition - fragPosition);
  vec3 V = normalize(viewPosition - fragPosition);
  vec3 R = reflect(-L, N);

  // specular intensity
  float spec = pow(max(dot(V, R), 0.0), shininess);
  vec3 specular = spec * lightColour;

  outputColour = vec4(specular, 1.0);
}

