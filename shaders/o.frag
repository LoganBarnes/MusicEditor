#version 410 core


in vec3 lookPos;
in vec3 posit;


out vec4 fragColor;

uniform vec3 eyeV;
uniform vec3 centr;
uniform float rad;

uniform sampler2D tex;
uniform int useTexture = 0;

void main(){
//    vec3 texColor = texture(tex, texcoords).rgb;
//    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
//    fragColor = vec4(clr * texColor, 1);

    float div = ((3.0 * rad)/4.0);
    vec3 lookpos = lookPos * div;
    float distIn = (((lookpos.x - posit.x) * (lookpos.x - posit.x)) + ((lookpos.y - posit.y) * (lookpos.y - posit.y)) + ((lookpos.z - posit.z) * (lookpos.z - posit.z)));
    float eucDist = sqrt(distIn);
    float divir = clamp((eucDist/rad), 0.0, 1.0);
    fragColor = vec4(vec3(1.0) * divir, 1.0);
}
