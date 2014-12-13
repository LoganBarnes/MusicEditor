#version 330 core

in vec3 position;               //Position in object space
in vec3 normal;   // Normal of the vertex
in vec2 texCoord; // UV texture coordinates


//// Transformation matrices
uniform mat4 model;             //model matrix
uniform mat4 view;              //view matrix
uniform mat4 projection;        //projection matrix


void main()
{

    gl_Position = projection*view*model*(vec4(position,1.0));

}
