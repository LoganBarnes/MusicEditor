#include "filtershape.h"
#include <math.h>

FilterShape::FilterShape(int p1, int p2, float p3, float radius)
{
    setParamMax(3, 3, 1.f);
    setParam1(p1);
    setParam2(p2);
    setParam3(p3);

    m_radius = radius;
    m_R = m_radius - (m_radius * m_p3 / 100.f);
    m_r = m_radius - m_R;
}


FilterShape::~FilterShape()
{
}


void FilterShape::calcVerts()
{
    // 3 verts per triangle * slices *
    // squares per slice * 2 * 2 for normals
    m_numVerts = (((m_p2 + 2) * 2 * m_p1) - 2);
    int size = m_numVerts * 8;
    m_vertexData = new GLfloat[size];

    // calc the radius to the center point and the
    // radius from the center ring to the surface
    m_R = m_radius - (m_radius * m_p3 / 100.f);
    m_r = m_radius - m_R;

    float prev = 0;
    float curr;

    float prevU = 1.f;
    float currU;

    int index = 0;

    // iterate through the slices
    for (int i = 1; i <= m_p1; i++) {
        curr = i * M_PI * 2.f / m_p1;

        currU = 1.f - ((i * 1.f) / m_p1);

        make3Dslice(&index, prev, curr, i == 1, i == m_p1, prevU, currU);

        prev = curr;
        prevU = currU;
    }
}


void FilterShape::make3Dslice(int *index, float phiL, float phiR, bool first, bool last, float leftU, float rightU)
{
    float theta;

    // center points
    glm::vec3 centerL = glm::vec3(cos(phiL), sin(phiL), 0) * m_R;
    glm::vec3 centerR = glm::vec3(cos(phiR), sin(phiR), 0) * m_R;

    // parametric torus equation
    glm::vec3 vl = glm::vec3((m_R + m_r * cos(M_PI)) * cos(phiL),
                        (m_R + m_r * cos(M_PI)) * sin(phiL),
                        m_r * sin(M_PI));

    glm::vec3 nl = glm::normalize(vl - centerL);

    glm::vec3 vr, nr;

    // texture coords
    glm::vec2 texl = glm::vec2(leftU, 0);
    glm::vec2 texr = glm::vec2(rightU, 0);

    // double the first point if it's not the first point on the shape
    if (!first)
        addVertex(index, vl, nl);

    for (int i = 0; i <= m_p2; i++) {
        theta = i * M_PI * 2.f / m_p2 + M_PI;

        // parametric torus equation
        vl = glm::vec3((m_R + m_r * cos(theta)) * cos(phiL),
                            (m_R + m_r * cos(theta)) * sin(phiL),
                            m_r * sin(theta));
        vr = glm::vec3((m_R + m_r * cos(theta)) * cos(phiR),
                            (m_R + m_r * cos(theta)) * sin(phiR),
                            m_r * sin(theta));

        nl = glm::normalize(vl - centerL);
        nr = glm::normalize(vr - centerR);

        texl.y = 1.f - (i * 1.f / m_p2);
        texr.y = 1.f - (i * 1.f / m_p2);

        addVertexT(index, vl, nl, texl);
        addVertexT(index, vr, nr, texr);
    }
    // double the last point if it's not the last point on the shape
    if (!last)
        addVertex(index, vr, nr);

}

