#version 410 core

in vec3 position;
in vec3 normal;

out vec3 vertex;	// The position of the vertex, in eye space
out vec3 vertexToEye;	// Vector from the vertex to the eye
out vec3 eyeNormal;	// Normal of the vertex, in eye space
out vec3 vertexToLight; // Vector from the vertex to the light

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform vec4 lightPosition;

// music function
uniform int functionSize = 0;
uniform float function[25]; // max/msp doesn't send data larger than 25

void calcVertex(inout vec3 v, inout vec3 n) {

    if (functionSize == 0) {
        return;
    }

    float angle = acos(dot(normalize(position), vec3(0, -1, 0)));

    float sizeMinus = functionSize - 1.0;
    float di = (angle / 3.1415926535897932384626433832795) * functionSize - 0.5;
    float f = mod(di, 1.0);
    di -= f;
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

    float a = atan(tangent.y, tangent.x);

    v += n * curve;

    vec3 axis = normalize(cross(vec3(0, 1, 0), n));

    float s = sin(-a);
    float c = cos(-a);
    float oc = 1.0 - c;

     mat4 rot = mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
                     oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
                     oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
                     0.0, 0.0, 0.0, 1.0);

    n = vec3(rot * vec4(normalize(n), 0));
}

void main() 
{
    vec3 pos = vec3(position);
    vec3 norm = vec3(normal);

    calcVertex(pos, norm);

    vertex = ((view*model)*(vec4(pos, 1.0))).xyz;
    vec4 camLightPos = view * model * lightPosition;
    vertexToLight = normalize(camLightPos.xyz - vertex);
    eyeNormal = normalize(mat3(transpose(inverse(view*model))) * norm);
    vertexToEye = -normalize(vertex);
    gl_Position = projection*view*model*vec4(pos,1.0);
}
