#include "musicshape.h"
#include "udphandler.h"
#include <glm/gtx/rotate_vector.hpp>

MusicShape::MusicShape(int p1, int p2, float radius, GLuint shader, QObject *parent) :
    QObject(parent)
{
    setParamMax(1, 2, -1);
    setParam1(p1);
    setParam2(p2);

    m_radius = radius;
    m_rate = 1;
    m_udp = new UDPHandler(this);
    m_shader = shader;
}

MusicShape::~MusicShape()
{
}


void MusicShape::calcVerts()
{
    int p2 = m_p2 * 2;
    int p1 = m_p1 * 2;
    m_numVerts = p1 * 2 * p2 * 2 + 2;
    int size = m_numVerts * 8;
    m_vertexData = new GLfloat[size];

    // for rotation
    float angleSpacing = 2.f * M_PI / p2;
    float cosine = cos(angleSpacing);
    float sine = sin(angleSpacing);

    float x = m_radius;
    float y = 0;
    float temp;

    glm::vec2 outer = glm::vec2(x, y);
    glm::vec2 inner;

    // rotation matrix
    temp = x;
    x = cosine * x - sine * y;
    y = sine * temp + cosine * y;

    int indexf = 0;
    int indexb = (m_numVerts / 2) * 8;

    float scale;
    glm::vec4 ef1, ef2;

    ef1 = f(0, 0);
    glm::vec3 center = glm::vec3(0, 0, ef1.w);

    ef2 = fv(outer);
    addVertex(&indexf, glm::vec3(outer, ef2.w), glm::vec3(ef2));
    addVertex(&indexb, glm::vec3(outer, -ef2.w), glm::vec3(ef2.x, ef2.y, -ef2.z));

    for (int i = 1; i <= p2; i++)
    {

        if (i % 2 == 0)
        {
            outer.x = x;
            outer.y = y;

            float s = 1.f / p1;
            for (int j = 1; j <= p1; j++)
            {
                scale = j * s;

                ef1 = fv(inner * scale);
                ef2 = fv(outer * scale);

                // frontside
                addVertex(&indexf, glm::vec3(inner * scale, ef1.w), glm::vec3(ef1));
                addVertex(&indexf, glm::vec3(outer * scale, ef2.w), glm::vec3(ef2));

                // backside
                addVertex(&indexb, glm::vec3(inner * scale, -ef1.w), glm::vec3(ef1.x, ef1.y, -ef1.z));
                addVertex(&indexb, glm::vec3(outer * scale, -ef2.w), glm::vec3(ef2.x, ef2.y, -ef2.z));
            }
        }
        else
        {
            inner.x = x;
            inner.y = y;

            ef2 = fv(inner);
            addVertex(&indexf, glm::vec3(inner, ef2.w), glm::vec3(ef2));
            addVertex(&indexb, glm::vec3(inner, -ef2.w), glm::vec3(ef2.x, ef2.y, -ef2.z));

            float s = 1.f / p1;

            for (int j = p1 - 1; j > 0; j--)
            {
                scale = j * s;

                ef1 = fv(outer * scale);
                ef2 = fv(inner * scale);

                // frontside
                addVertex(&indexf, glm::vec3(outer * scale, ef1.w), glm::vec3(ef1));
                addVertex(&indexf, glm::vec3(inner * scale, ef2.w), glm::vec3(ef2));

                // backside
                addVertex(&indexb, glm::vec3(outer * scale, -ef1.w), glm::vec3(ef1.x, ef1.y, -ef1.z));
                addVertex(&indexb, glm::vec3(inner * scale, -ef2.w), glm::vec3(ef2.x, ef2.y, -ef2.z));
            }
            addVertex(&indexf, center, glm::vec3(0, 0, 1));
            addVertex(&indexb, -center, glm::vec3(0, 0, -1));
        }

        // rotation matrix
        temp = x;
        x = cosine * x - sine * y;
        y = sine * temp + cosine * y;

    }
}


void MusicShape::setFunction(QList<float> function)
{
    m_function = function;
    calcVerts();
    updateGL(m_shader);
    cleanUp();
}


bool MusicShape::animate()
{
    return true;
}



/**
 * @brief MusicShape::f the bezier curve used to draw the shape
 * @param x - the x location
 * @param y - the y location
 * @return the z location based on the function
 */
glm::vec4 MusicShape::f(float x, float y)
{
    return fv(glm::vec2(x, y));
}


/**
 * @brief MusicShape::f the bezier curve used to draw the shape
 * @param x - the x location
 * @param y - the y location
 * @return the z location based on the function
 */
glm::vec4 MusicShape::fv(glm::vec2 v)
{
    m_function.clear();
    m_function.append(0.5f);
    m_function.append(0.1f);
    m_function.append(0.3f);
    m_function.append(0.0f);
    m_function.append(0.0f);

    if (m_function.isEmpty())
        return glm::vec4(0.f);

    double sizeMinus = m_function.size() - 1.f;
    double di = sqrt(v.x*v.x + v.y*v.y) * m_function.size() - 0.5f;
    float f = modf(di, &di);
    int li, ri;
    float t;
    glm::vec2 mid, left, right;
    if (f < 0.5f) {
        t = f + 0.5f;

        left.x = di - 0.5f;
        mid.x = di;
        right.x = di + 0.5f;

        li = (int) glm::max(0.0, di - 1.0);
        ri = (int) glm::min(di + 1.0, sizeMinus);
    } else {
        t = f - 0.5f;

        left.x = di + 0.5f;
        mid.x = di + 1.f;
        right.x = di + 1.5f;

        li = (int) glm::max(0.0, di);
        ri = (int) glm::min(di + 2.f, sizeMinus);
        di = glm::min(di + 1.f, sizeMinus);
    }
    mid.y = m_function.value((int) di);
    left.y = (m_function.value(li) + mid.y) / 2.f;
    right.y = (mid.y+ m_function.value(ri)) / 2.f;

    float t_1 = 1.f - t;
    float curve = t_1 * (t_1 * left.y + t * mid.y) + t * (t_1 * mid.y + t * right.y);

    glm::vec2 tangent = 2 * t_1 * (mid - left) + 2 * t * (right - mid);
    tangent.x /= sizeMinus;

    glm::vec2 n = glm::rotate(tangent, (float)(M_PI / 2.0));
    v = glm::normalize(v);
    glm::vec3 norm = glm::normalize(glm::vec3(v.x * n.x, v.y * n.x, n.y));

    return glm::vec4(norm.x, norm.y, norm.z, curve);

}

