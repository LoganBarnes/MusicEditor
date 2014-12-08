#include "shape.h"

Shape::Shape()
{
    m_numVerts = 4;
    m_vertexData = NULL;
    m_vaoID = 0;
    m_vboID = 0;

    setParamMax(-1, -1, -1.f);
}

Shape::~Shape()
{
    cleanUp();
    if (m_vaoID)
        glDeleteVertexArrays(1, &m_vaoID);
    if (m_vboID)
        glDeleteBuffers(1, &m_vboID);
}


void Shape::setParamMax(int p1, int p2, float p3)
{
    m_p1max = p1;
    m_p2max = p2;
    m_p3max = p3;
}

void Shape::setParam1(int param1)
{
    m_p1 = param1;
    if (m_p1 < m_p1max)
        m_p1 = m_p1max;
}

void Shape::setParam2(int param2)
{
    m_p2 = param2;
    if (m_p2 < m_p2max)
        m_p2 = m_p2max;
}

void Shape::setParam3(float param3)
{
    m_p3 = param3;
    if (m_p3 < m_p3max)
        m_p3 = m_p3max;
}

bool Shape::usesParam(int num)
{
    if (num == 1)
        return m_p1max > -1;
    if (num == 2)
        return m_p2max > -1;
    if (num == 3)
        return m_p3max > -1.f;
    return false;
}


void Shape::calcVerts()
{
    int size = m_numVerts * 8;
    m_vertexData = new GLfloat[size];

    int index = 0;
    glm::vec3 norm = glm::vec3(0, 0, 1);

    addVertexT(&index, glm::vec3(-.5f, -.5f, 0), norm, glm::vec2(0.f, 1.f));
    addVertexT(&index, glm::vec3(.5f, -.5f, 0), norm, glm::vec2(1.f, 1.f));
    addVertexT(&index, glm::vec3(-.5, .5, 0), norm, glm::vec2(0.f, 0.f));
    addVertexT(&index, glm::vec3(.5, .5, 0), norm, glm::vec2(1.f, 0.f));
}


void Shape::updateGL(GLuint shader)
{
    if (m_vaoID)
        glDeleteVertexArrays(1, &m_vaoID);
    if (m_vboID)
        glDeleteBuffers(1, &m_vboID);

    // Initialize the vertex array object.
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // Initialize the vertex buffer object.
    glGenBuffers(1, &m_vboID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glBufferData(GL_ARRAY_BUFFER, 8 * m_numVerts * sizeof(GLfloat), m_vertexData, GL_STATIC_DRAW);

    GLuint position = glGetAttribLocation(shader, "position");
    GLuint normal = glGetAttribLocation(shader, "normal");
    GLuint texCoord = glGetAttribLocation(shader, "texCoord");

    glEnableVertexAttribArray(position);
    glVertexAttribPointer(
        position,
        3,                              // Num coordinates per position
        GL_FLOAT,                       // Type
        GL_FALSE,                       // Normalized
        sizeof(GLfloat) * 8,            // Stride
        (void*) 0                       // Array buffer offset
    );
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(
        normal,
        3,                              // Num coordinates per normal
        GL_FLOAT,                       // Type
        GL_TRUE,                        // Normalized
        sizeof(GLfloat) * 8,            // Stride
        (void*) (sizeof(GLfloat) * 3)   // Array buffer offset
    );
    glEnableVertexAttribArray(texCoord);
    glVertexAttribPointer(
        texCoord,
        2,                              // Num coordinates per position
        GL_FLOAT,                       // Type
        GL_TRUE,                        // Normalized
        sizeof(GLfloat) * 8,            // Stride
        (void*) (sizeof(GLfloat) * 6)   // Array buffer offset
    );

    // Unbind buffers.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Shape::cleanUp()
{
    if (m_vertexData) {
        delete[] m_vertexData;
        m_vertexData = NULL;
    }
}


void Shape::render()
{
        glBindVertexArray(m_vaoID);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numVerts); /* Number of vertices to draw (w/o normals) */
        glBindVertexArray(0);
}


void Shape::transformAndRender(GLuint shader, glm::mat4 trans)
{
        glBindVertexArray(m_vaoID);
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(trans));
        glUniform1i(glGetUniformLocation(shader, "functionSize"), m_function.size());
        glUniform1fv(glGetUniformLocation(shader, "function"), m_function.size(), m_function.data());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numVerts); /* Number of vertices to draw (w/o normals) */
        glBindVertexArray(0);
}


void Shape::addVertex(int *i, glm::vec3 v, glm::vec3 norm)
{
    m_vertexData[(*i)++] = v.x;
    m_vertexData[(*i)++] = v.y;
    m_vertexData[(*i)++] = v.z;
    m_vertexData[(*i)++] = norm.x;
    m_vertexData[(*i)++] = norm.y;
    m_vertexData[(*i)++] = norm.z;
    m_vertexData[(*i)++] = 0;
    m_vertexData[(*i)++] = 0;
}


void Shape::addVertexT(int *i, glm::vec3 v, glm::vec3 norm, glm::vec2 tex)
{
    m_vertexData[(*i)++] = v.x;
    m_vertexData[(*i)++] = v.y;
    m_vertexData[(*i)++] = v.z;
    m_vertexData[(*i)++] = norm.x;
    m_vertexData[(*i)++] = norm.y;
    m_vertexData[(*i)++] = norm.z;
    m_vertexData[(*i)++] = tex.x;
    m_vertexData[(*i)++] = tex.y;
}


void Shape::printVert(glm::vec3 v)
{
    cout << "(" << v.x;
    cout << ", " << v.y;
    cout << ", " << v.z;
    cout << ")" << endl;
}

bool Shape::animate()
{
    return false;
}


glm::vec2 Shape::mapPoints(const glm::vec2 val, const glm::vec2 oldMin, const glm::vec2 oldMax,
                       const glm::vec2 newMin, const glm::vec2 newMax)
{
    return (val - oldMin) / (oldMax - oldMin) * (newMax - newMin) + oldMin;
}


float Shape::map(const float val, const float oldMin, const float oldMax, const float newMin, const float newMax)
{
    return (val - oldMin) / (oldMax - oldMin) * (newMax - newMin) + oldMin;
}


