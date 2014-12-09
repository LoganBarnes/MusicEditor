#version 330

in vec3 position;

uniform mat4 projection;
uniform mat4 view;

out vec3 texcoords;

void main () {
  texcoords = position;
  gl_Position = projection * view * vec4(position, 1.0);
}
