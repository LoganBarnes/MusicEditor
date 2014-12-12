#version 410

in vec3 texcoords;

uniform samplerCube envMap;

out vec4 fragColor;

void main () {
	fragColor = texture(envMap, texcoords);
}
