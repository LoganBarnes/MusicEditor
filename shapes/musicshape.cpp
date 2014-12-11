#include "musicshape.h"
#include "udphandler.h"
#include <math.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

MusicShape::MusicShape(int p1, int p2, float radius, QObject *parent) :
    QObject(parent)
{
    setParamMax(2, 3, -1);
    setParam1(p1);
    setParam2(p2);

    m_radius = radius;
}


MusicShape::~MusicShape()
{
}


void MusicShape::calcVerts()
{
    // ((slices * verts per slice) - first and last vert) * 2 for normals
    m_numVerts = ((m_p2 * (m_p1 + 1) * 2) - 2);
    int size = m_numVerts * 8; // 3 points per vertex
    m_vertexData = new GLfloat[size];

    float prev = 0;
    float curr;

    int index = 0;

    glm::vec3 topN = glm::vec3(0, 1, 0);
    glm::vec3 bottomN = glm::vec3(0, -1, 0);

    glm::vec3 top = glm::vec3(0, m_radius, 0);
    glm::vec3 bottom = glm::vec3(0, -m_radius, 0);

    // iterate through the slices
    for (int i = 1; i <= m_p2; i++) {
        curr = i * M_PI * 2.f / m_p2;

        // top point
        glm::vec2 tex = glm::vec2(1.f - (prev / (2 * M_PI)), 0.f);
        addVertexT(&index, top, topN, tex);

        make3Dslice(&index, curr, prev);

        // bottom point
        tex.y = 1.f;
        addVertexT(&index, bottom, bottomN, tex);

        // repeat the last point of this slice and the first point of the next
        // slice so the renderer won't connect the two points
        if (i != m_p2) {
            addVertex(&index, bottom, bottomN);
            addVertex(&index, top, topN);
        }

        prev = curr;
    }
}


void MusicShape::make3Dslice(int *index, float thetaL, float thetaR)
{
    double spacing = M_PI / m_p1;
    float phi;

    // iterate through sub blocks of slice
    for (int i = 1; i < m_p1; i++) {
        phi = i * spacing;
        calcSliceSeg(index, thetaL, thetaR, phi);
    }
}

void MusicShape::calcSliceSeg(int *index, float thetaL, float thetaR, float phi)
{

    // parametric sphere equations
    glm::vec3 vl = glm::vec3(m_radius * sin(phi) * cos(thetaL),
                        m_radius * cos(phi),
                        m_radius * sin(phi) * sin(thetaL));
    glm::vec3 vr = glm::vec3(m_radius * sin(phi) * cos(thetaR),
                        m_radius * cos(phi),
                        m_radius * sin(phi) * sin(thetaR));

    glm::vec2 texl = glm::vec2(1.f - thetaL / (2 * M_PI), phi / M_PI);
    glm::vec2 texr = glm::vec2(1.f - thetaR / (2 * M_PI), phi / M_PI);

    glm::vec3 nl = glm::normalize(vl);
    glm::vec3 nr = glm::normalize(vr);

    addVertexT(index, vl, nl, texl);
    addVertexT(index, vr, nr, texr);
}


bool MusicShape::animate()
{
    return true;
}


void MusicShape::transformAndRender(GLuint shader, glm::mat4 trans)
{
    Shape::transformAndRender(shader, trans);
}


//glm::vec4 MusicShape::f(glm::vec3 *v, glm::vec3 *n)
//{
//    if (m_function.isEmpty())
//        return glm::vec4(0,0,0, 0.f);

//    float angle = glm::acos(glm::dot(glm::normalize(*v), glm::vec3(0, 1, 0)));

//    double sizeMinus = m_function.size() - 1.f;
//    double di = (angle / M_PI) * m_function.size() - 0.5f;
//    float f = modf(di, &di);
//    int li, ri;
//    float t;
//    glm::vec2 mid, left, right;
//    if (f < 0.5f) {
//        t = f + 0.5f;

//        left.x = di - 0.5f;
//        mid.x = di;
//        right.x = di + 0.5f;

//        li = (int) glm::max(0.0, di - 1.0);
//        ri = (int) glm::min(di + 1.0, sizeMinus);
//    } else {
//        t = f - 0.5f;

//        left.x = di + 0.5f;
//        mid.x = di + 1.f;
//        right.x = di + 1.5f;

//        li = (int) glm::max(0.0, di);
//        ri = (int) glm::min(di + 2.f, sizeMinus);
//        di = glm::min(di + 1.f, sizeMinus);
//    }
//    mid.y = m_function.value((int) di);
//    left.y = (m_function.value(li) + mid.y) / 2.f;
//    right.y = (mid.y+ m_function.value(ri)) / 2.f;

//    float t_1 = 1.f - t;
//    float curve = t_1 * (t_1 * left.y + t * mid.y) + t * (t_1 * mid.y + t * right.y);

//    glm::vec2 tangent = 2 * t_1 * (mid - left) + 2 * t * (right - mid);
//    tangent.x /= sizeMinus;

////    glm::vec2 n = glm::vec2(-tangent.y, tangent.x);

//    float a = -atan2(tangent.y, tangent.x);

//    *v += *n * curve;

//    glm::vec3 axis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), *n));

//    float s = sin(-a);
//    float c = cos(-a);
//    float oc = 1.0 - c;

//     glm::mat4 rot = glm::mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
//                                oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
//                                oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
//                                0.0, 0.0, 0.0, 1.0);

//    *n = glm::vec3(rot * glm::vec4(*n, 0));
////     *n = glm::rotate(*n, a, axis);


//    return glm::vec4(0, 0, 0, a);

//}

float MusicShape::collisionDetector(glm::vec3 cent1, glm::vec3 cent2, float rad1, float rad2) {
   // float inner = (powf((cent1.x - cent2.x), 2.0f) + powf((cent1.y - cent2.y), 2.0f) + powf((cent1.z - cent2.z), 2.0f));
    float eucDist = euclideanDist(cent1, cent2);
    if (eucDist == 0.0f) {
        return (rad1 + rad2);
    }
    if (eucDist < (rad1 + rad2)) {
        float radDiff1 = (rad2 - rad1);
        float radDiff2 = (rad1 - rad2);
        return (eucDist - radDiff1 - radDiff2);
    }
    else {
        return 0.0f;
    }
}

float MusicShape::getRadius() {
    return m_radius;
}

void MusicShape::setRadius(float rad) {
    m_radius = rad;
}



float MusicShape::calcIntersect(glm::vec3 eye, glm::vec3 dir) {

    float aX = (dir.x * dir.x);
    float bX = (2.0f * eye.x * dir.x);
    float cX = (eye.x * eye.x);

    float aY = (dir.y * dir.y);
    float bY = (2.0f * eye.y * dir.y);
    float cY = (eye.y * eye.y);

    float aZ = (dir.z * dir.z);
    float bZ = (2.0f * eye.z * dir.z);
    float cZ = (eye.z * eye.z);

    float a = (aX + aY + aZ);
    float b = (bX + bY + bZ);
    float c = (cX + cY + cZ - 0.25f);

    float retInt = -1.0f;

    bool setT = false;
    glm::vec2 tInt = solveQuadr(a, b, c);
    if (tInt.x > 0.0f) {
        float yVal = (eye.y + (tInt.x * dir.y));
        if (yVal >= -m_radius && yVal <= m_radius) {
            if (setT) {
                if (retInt > tInt.x) {
                    retInt = tInt.x;
                }
            }
            else {
                retInt = tInt.x;
                setT = true;
            }
        }
    }
    if (tInt.y > 0.0f) {
        float yVal = (eye.y + (tInt.y * dir.y));
        if (yVal >= -0.5001f && yVal <= 0.5001f) {
            if (setT) {
                if (retInt > tInt.y) {
                    retInt = tInt.y;
                }
            }
            else {
                retInt = tInt.y;
                setT = true;
            }
        }
    }

    return retInt;

}
