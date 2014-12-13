#version 410 core

uniform mat4 shadowMapProjections[6];
uniform mat4 waterModel;
uniform vec3 centr;

layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 18) out;

in vec4 pos[3];
//in vec3 color[3];
//in vec2 texc[3];
in vec3 lokpos[3];
in vec3 posish[3];

out vec3 lookPos;
out vec3 posit;
//out vec3 clr;
//out vec2 texcoords;

 void main()
{

     lookPos = normalize(-vec3(waterModel[3]) - centr);
     for(int j = 0; j < 6; ++j) {
         gl_Layer = j;
         for(int i = 0; i < 3; ++i) {
//             clr = color[i];
//             texcoords = texc[i];
             posit = posish[i];
             gl_Position = shadowMapProjections[j] * waterModel * pos[i];
             EmitVertex();
         }
         EndPrimitive();
     }
}
