#version 330 core

in vec3 vertex;                 // The position of the vertex, in eye space
in vec3 vertexToLight;          // Vector from the vertex to the light
in vec3 vertexToEye;            // Vector from the vertex to the eye
in vec3 eyeNormal;		// Normal of the vertex, in eye space

//uniform samplerCube envMap;	// The cube map containing the environment to reflect
uniform vec3 ambient_color;		// The ambient channel of the color to reflect
uniform vec3 diffuse_color;		// The diffuse channel of the color to reflect
uniform vec3 specular_color;		// The specular channel of the color to reflect

uniform mat4 model;             //model matrix
uniform mat4 view;              //view matrix
//uniform mat4 mvp;               //model view projection matrix
    
uniform float r0;		// The Fresnel reflectivity when the incident angle is 0
uniform float m;		// The roughness of the material

out vec4 fragColor;

void main()
{
    vec4 ambient = vec4(ambient_color, 1);
    vec4 diffuse = vec4(diffuse_color, 1);
    vec4 specular = vec4(specular_color, 1);

    vec3 n = normalize(eyeNormal);
    vec3 l = normalize(vertexToLight);
    vec3 eyeToVertex = normalize(vertex); //remember we are in eye space!
    
    fragColor = vec4(0.0);
    fragColor += diffuse * max(0.0, dot(n, l));
    fragColor += ambient;
    
    vec3 h = normalize(vertexToLight + vertexToEye);
    float eDotH = dot(eyeToVertex, h);
    float hDotN = dot(h, n);
    float eDotN = dot(eyeToVertex, n);
    float lDotN = dot(l, n);
    
    float alpha = acos(dot(n, h));
    float D = exp(-pow(tan(alpha), 2.0) / (m * m)) / (4.0 * m * m * pow(cos(alpha), 4.0));
    
    float F = r0 + (1.0 - r0) * pow(1.0 - dot(n, normalize(vertexToEye)), 5.0);
    
    float G = min(1.0, min(2.0 * hDotN * eDotN / eDotH, 2 * hDotN * lDotN / eDotH));
    
    float k_s = max(0.0, D * F * G / eDotN);
    
    fragColor += k_s * specular;
    
    vec4 r_world = inverse(view * model) * vec4(reflect(eyeToVertex, n), 0.0);
    
//    vec4 rcolor = texture(envMap, normalize(r_world).xyz);
    vec4 rcolor = vec4(0.f);
    
    fragColor = mix(fragColor, rcolor, F);

}
