#version 410 core

uniform mat4 shadowMapProjections[6];
uniform mat4 waterModel;

layout(lines, invocations = 1) in;
layout(line_strip, max_vertices = 12) out;

in vec4 pos[2];

 void main()
{
     for(int j = 0; j < 6; ++j) {
         gl_Layer = j;
         for(int i = 0; i < 2; ++i) {
             gl_Position = shadowMapProjections[j] * waterModel * pos[i];
             EmitVertex();
         }
         EndPrimitive();
     }
}
