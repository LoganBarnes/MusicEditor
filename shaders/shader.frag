#version 410 core

in vec3 clr;
in vec2 texcoords;

out vec4 fragColor;

uniform sampler2D tex;
uniform int useTexture = 0;

void main(){
    vec3 texColor = texture(tex, texcoords).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
    fragColor = vec4(clr * texColor, 1);
}
