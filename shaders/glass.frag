#version 330

in vec3 vertex;         // The position of the vertex, in eye space!
in vec3 vertexToEye;    // Vector from the vertex to the eye
in vec3 eyeNormal;	    // Normal of the vertex, in eye space!

uniform float r0;	// The R0 value to use in Schlick's approximation
uniform vec3  eta;	// Contains one eta for each channel (use eta.r, eta.g, eta.b in your code)

uniform mat4 view;
uniform mat4 model;

uniform samplerCube envMap;

out vec4 fragColor;

void main()
{
    vec3 n = normalize(eyeNormal);
    vec3 eyeToVertex = -vertexToEye; //remember we are in eye space!
    
    fragColor = vec4(0.0);    
    float F = r0 + (1.0 - r0) * pow(1.0 - dot(n, normalize(-eyeToVertex)), 5.0);

    mat4 inv = inverse(view);

    // reflection
    vec4 reflectVec = inv * vec4(reflect(eyeToVertex, n), 0.0);

    // refraction
    vec4 refractVecR = inv * vec4(refract(eyeToVertex, n, eta.r), 0.0);
    vec4 refractVecG = inv * vec4(refract(eyeToVertex, n, eta.g), 0.0);
    vec4 refractVecB = inv * vec4(refract(eyeToVertex, n, eta.b), 0.0);
    
    // sample cube
    vec4 reflectColor = texture(envMap, normalize(reflectVec).xyz);

    vec4 refractColor = vec4(1);
    refractColor.r = texture(envMap, normalize(refractVecR).xyz).r;
    refractColor.g = texture(envMap, normalize(refractVecG).xyz).g;
    refractColor.b = texture(envMap, normalize(refractVecB).xyz).b;
    
    // combine
    fragColor = mix(refractColor, reflectColor, F);
    fragColor.b *= 1.15; // add a slight blue hue
    fragColor = clamp(fragColor, 0.0, 1.0);
//    fragColor = vec4(1);
}
