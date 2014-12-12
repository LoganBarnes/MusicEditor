#version 410 core

in vec3 position;               //Position in object space

out vec4 pos;

//// Transformation matrices
uniform mat4 model;             //model matrix
uniform mat4 view;              //view matrix
uniform mat4 projection;        //projection matrix


void main()
{
	pos = model * vec4(position, 1.0);
    gl_Position = projection*view*model*(vec4(position,1.0));
}
