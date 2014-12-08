#include "grid.h"

Grid::Grid(float radius)
{
    m_numVerts = 0;
    m_vertexData = NULL;
    m_vaoID = 0;
    m_vboID = 0;

    m_radius = radius;
}

Grid::~Grid()
{
    cleanUp();
    if (m_vaoID)
        glDeleteVertexArrays(1, &m_vaoID);
    if (m_vboID)
        glDeleteBuffers(1, &m_vboID);
}


void Grid::calcVerts()
{
    m_numVerts = (12 * 2) * 2 + 3;
    int size = m_numVerts * 8;
    m_vertexData = new GLfloat[size];

    int index = 0;
    glm::vec3 norm = glm::vec3(0, 0, 1);

    int lines = 12; // should be even
    float spacing = m_radius * 2.f / lines;
    float x;
    for (int i = 0; i < lines; i += 2) {
        x = i * spacing - m_radius;

        addVertex(&index, glm::vec3( x, -1.0,  -m_radius), norm);
        addVertex(&index, glm::vec3( x, -1.0,  m_radius), norm);

        x += spacing;

        addVertex(&index, glm::vec3( x, -1.0,  m_radius), norm);
        addVertex(&index, glm::vec3( x, -1.0,  -m_radius), norm);

    }
    addVertex(&index, glm::vec3(  m_radius, -1.0,  -m_radius), norm);
    addVertex(&index, glm::vec3(  m_radius, -1.0,   m_radius), norm);
    addVertex(&index, glm::vec3( -m_radius, -1.0,   m_radius), norm);

    float z;
    for (int i = lines - 1; i > 0; i -= 2) {
        z = i * spacing - m_radius;

        addVertex(&index, glm::vec3( -m_radius, -1.0, z), norm);
        addVertex(&index, glm::vec3(  m_radius, -1.0, z), norm);

        z -= spacing;

        addVertex(&index, glm::vec3(  m_radius, -1.0, z), norm);
        addVertex(&index, glm::vec3( -m_radius, -1.0, z), norm);
    }

    cout << index << ", " << size << endl;
}


void Grid::updateGL(GLuint shader)
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
        3,                   // Num coordinates per position
        GL_FLOAT,            // Type
        GL_FALSE,            // Normalized
        sizeof(GLfloat) * 8, // Stride
        (void*) 0            // Array buffer offset
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

void Grid::cleanUp()
{
    if (m_vertexData) {
        delete[] m_vertexData;
        m_vertexData = NULL;
    }
}


void Grid::render()
{
        glBindVertexArray(m_vaoID);
        glDrawArrays(GL_LINE_STRIP, 0, m_numVerts); /* Number of vertices to draw (w/o normals) */
        glBindVertexArray(0);
}


void Grid::transformAndRender(GLuint shader, glm::mat4 trans)
{
        glBindVertexArray(m_vaoID);
        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(trans));
        glDrawArrays(GL_LINE_STRIP, 0, m_numVerts); /* Number of vertices to draw (w/o normals) */
        glBindVertexArray(0);
}


void Grid::addVertex(int *i, glm::vec3 v, glm::vec3 norm)
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


void Grid::addVertexT(int *i, glm::vec3 v, glm::vec3 norm, glm::vec2 tex)
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


void Grid::printVert(glm::vec3 v)
{
    cout << "(" << v.x;
    cout << ", " << v.y;
    cout << ", " << v.z;
    cout << ")" << endl;
}

bool Grid::animate()
{
    return false;
}


glm::vec2 Grid::mapPoints(const glm::vec2 val, const glm::vec2 oldMin, const glm::vec2 oldMax,
                       const glm::vec2 newMin, const glm::vec2 newMax)
{
    return (val - oldMin) / (oldMax - oldMin) * (newMax - newMin) + oldMin;
}


float Grid::map(const float val, const float oldMin, const float oldMax, const float newMin, const float newMax)
{
    return (val - oldMin) / (oldMax - oldMin) * (newMax - newMin) + oldMin;
}


