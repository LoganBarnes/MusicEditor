#include "musicshape.h"
#include "udphandler.h"
#include <math.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

MusicShape::MusicShape(int p1, int p2, float radius, int port, QObject *parent) :
    QObject(parent)
{
    setParamMax(2, 3, -1);
    setParam1(p1);
    setParam2(p2);

    m_radius = radius;
    m_udp = new UDPHandler(this, port);
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


void MusicShape::setFunction(QVector<float> function)
{
    m_function = QVector<float>(function);
}


void MusicShape::transformAndRender(GLuint shader, glm::mat4 trans)
{
    Shape::transformAndRender(shader, trans);
}

