
#version 330 core

#define PI 3.1415926535897932384626433832795

in vec3 position; // Position of the vertex
in vec3 normal;   // Normal of the vertex
in vec2 texCoord; // UV texture coordinates
in float arrowOffset; // Sideways offset for billboarded normal arrows

out vec3 color; // Computed color for this vertex
out vec2 texc;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
uniform vec3 lightColors[MAX_LIGHTS];

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;
uniform float repeatU;
uniform float repeatV;

uniform bool useLighting;     // Whether to calculate lighting using lighting equation
uniform bool useArrowOffsets; // True if rendering the arrowhead of a normal for Shapes
uniform vec3 allBlack = vec3(1);
uniform vec3 allWhite = vec3(0);

// music function
uniform int functionSize = 0;
uniform float function[25]; // max/msp doesn't send data larger than 25

vec2 rotateVec2(vec2 v, float sine, float cosine)
{
    return vec2(v.x * cosine - v.y * sine, v.x * sine + v.y * cosine);
}

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);

    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;

    return mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
                oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
                oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
                0.0, 0.0, 0.0, 1.0);
}

void calcVertex(inout vec3 v, inout vec3 n) {

    if (functionSize == 0) {
        return;
    }

    float angle = acos(dot(normalize(position), vec3(0, 1, 0)));

    float sizeMinus = functionSize - 1.0;
    float di = (angle / PI) * functionSize - 0.5;
    float f = modf(di, di);
    int li, ri;
    float t;
    vec2 mid, left, right;
    if (f < 0.5) {
        t = f + 0.5;

        left.x = di - 0.5;
        mid.x = di;
        right.x = di + 0.5;

        li = int(max(0.0, di - 1.0));
        ri = int(min(di + 1.0, sizeMinus));
    } else {
        t = f - 0.5;

        left.x = di + 0.5;
        mid.x = di + 1.;
        right.x = di + 1.5;

        li = int(max(0.0, di));
        ri = int(min(di + 2.0, sizeMinus));
        di = min(di + 1.0, sizeMinus);
    }
    mid.y = function[int(di)];
    left.y = (function[li] + mid.y) / 2.f;
    right.y = (mid.y+ function[ri]) / 2.f;

    float t_1 = 1.0 - t;
    float curve = t_1 * (t_1 * left.y + t * mid.y) + t * (t_1 * mid.y + t * right.y);

    vec2 tangent = 2 * t_1 * (mid - left) + 2 * t * (right - mid);
    tangent.x /= sizeMinus;

    vec2 norm = rotateVec2(tangent, 1.0, 0.0);

//    float a = glm::angle(glm::normalize(n), glm::vec2(0, 1));
//    a = (n.x < 0 ? -a : a);

//    return glm::vec4(glm::normalize(v) * curve, a);
}

void main(){
    texc = vec2(texCoord.x * repeatU, texCoord.y * repeatV);

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

    vec4 position_worldSpace = m * vec4(position, 1.0);
    vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

    if (useArrowOffsets) {
        // Figure out the axis to use in order for the triangle to be billboarded correctly
        vec3 offsetAxis = normalize(cross(vec3(position_cameraSpace), vec3(normal_cameraSpace)));
        position_cameraSpace += arrowOffset * vec4(offsetAxis, 0);
    }

    gl_Position = p * position_cameraSpace;

    if (useLighting) {
        color = ambient_color.xyz; // Add ambient component

        for (int i = 0; i < MAX_LIGHTS; i++) {
            vec4 vertexToLight = vec4(0);
            // Point Light
            if (lightTypes[i] == 0) {
                vertexToLight = normalize(v * vec4(lightPositions[i], 1) - position_cameraSpace);
            } else if (lightTypes[i] == 1) {
                // Dir Light
                vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
            }

            // Add diffuse component
            float diffuseIntensity = max(0.0, dot(vertexToLight, normal_cameraSpace));
            color += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity);

            // Add specular component
            vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
            vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
            float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
            color += max (vec3(0), lightColors[i] * specular_color * specIntensity);
        }
    } else {
        color = ambient_color + diffuse_color;
    }
    color = clamp(color + allWhite, 0.0, 1.0) * allBlack;
}
