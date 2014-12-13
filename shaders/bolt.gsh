#version 410 core

layout(lines, invocations = 1) in;
layout(line_strip, max_vertices = 2) out;

in vec4 pos[2];

 void main()
{
     for(int i = 0; i < 2; ++i) {
         gl_Position = gl_in[i].gl_Position;
         EmitVertex();
     }
     EndPrimitive();
}
