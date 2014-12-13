#version 410 core

uniform mat4 shadowMapProjections[6];

layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 18) out;

in vec4 pos[3];
//in vec3 color[3];
//in vec2 texc[3];
in float divisr[3];

out float divir;
//out vec3 clr;
//out vec2 texcoords;

 void main()
{

     for(int j = 0; j < 6; ++j) {
         gl_Layer = j;
         for(int i = 0; i < 3; ++i) {
//             clr = color[i];
//             texcoords = texc[i];
             divir = divisr[i];
             gl_Position = shadowMapProjections[j] * pos[i];
             EmitVertex();
         }
         EndPrimitive();
     }
}
