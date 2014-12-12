#version 410

in vec3 vertex;         // The position of the vertex, in eye space!
in vec3 vertexToEye;    // Vector from the vertex to the eye
in vec3 eyeNormal;	// Normal of the vertex, in eye space!
in vec3 vertexToLight;   // Vector from the vertex to the light

uniform float r0;	// The R0 value to use in Schlick's approximation
uniform vec3  eta;	// Contains one eta for each channel (use eta.r, eta.g, eta.b in your code)
uniform float m;        // roughness of material

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

    // specular
    vec3 h = normalize(vertexToLight + vertexToEye);
    float eDotH = dot(eyeToVertex, h);
    float hDotN = dot(h, n);
    float eDotN = dot(eyeToVertex, n);
    float lDotN = dot(vertexToLight, n);

    float alpha = acos(dot(n, h));
    float D = exp(-pow(tan(alpha), 2.0) / (m * m)) / (4.0 * m * m * pow(cos(alpha), 4.0));

    float G = min(1.0, min(2.0 * hDotN * eDotN / eDotH, 2 * hDotN * lDotN / eDotH));

    float k_s = max(0.0, D * F * G / eDotN);

    fragColor += k_s * vec4(1.0);

    fragColor = clamp(fragColor, 0.0, 1.0);
//    fragColor = vec4(1);
}
