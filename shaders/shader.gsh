#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 color[3];
in vec2 texc[3];

out vec3 clr;
out vec2 texcoords;

 void main()
{
     for(int i = 0; i < 3; ++i) {
         clr = color[i];
         texcoords = texc[i];
         gl_Position = gl_in[i].gl_Position;
         EmitVertex();
     }
     EndPrimitive();
}
