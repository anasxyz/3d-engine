#version 420 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec4 fragColour;
in vec2 fragTexCoord;

out vec4 outputColour;

uniform vec3 lightPosition;
uniform vec3 viewPosition;
uniform vec3 lightColour;
uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

uniform bool useTexture;
uniform sampler2D texSampler;

uniform int isLightSource;

void main() {
	// if it's a light source just skip the lighting effects and display it with its true colour
  if (isLightSource == 1) {
    if (useTexture) {
      outputColour = texture(texSampler, fragTexCoord);     
	  } else {
      outputColour = fragColour; 
    }

    return;
  }

	vec3 objectColour = fragColour.rgb;
	vec3 baseColour = useTexture ? texture(texSampler, fragTexCoord).rgb : objectColour;

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
  vec3 specular = specularStrength * spec * lightColour;

	// all together
  vec3 phong = (ambient + diffuse + specular) * baseColour;
  outputColour = vec4(phong, 1.0);
}
