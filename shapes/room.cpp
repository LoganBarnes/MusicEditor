#include "room.h"
#include <QGLWidget>
#include <QFile>

//#define FRONT ":/images/front.png"
//#define BACK ":/images/back.png"
//#define BOTTOM ":/images/bottom.png"
//#define TOP ":/images/top.png"
//#define LEFT ":/images/left.png"
//#define RIGHT ":/images/right.png"

#define RIGHT ":/images/posx.jpg"
#define LEFT ":/images/negx.jpg"
#define TOP ":/images/posy.jpg"
#define BOTTOM ":/images/negy.jpg"
#define BACK ":/images/posz.jpg"
#define FRONT ":/images/negz.jpg"

const int cube_s = 128;

Room::Room(float radius)
{
    m_radius = radius;
}


Room::~Room()
{
}


void Room::init()
{
    float points[] = {
        -m_radius ,  m_radius,  m_radius, // 4
         m_radius ,  m_radius,  m_radius, // 3
        -m_radius , -m_radius,  m_radius, // 7
         m_radius , -m_radius,  m_radius, // 8

         m_radius , -m_radius, -m_radius, // 5
         m_radius ,  m_radius,  m_radius, // 3
         m_radius ,  m_radius, -m_radius, // 1

        -m_radius ,  m_radius,  m_radius, // 4
        -m_radius ,  m_radius, -m_radius, // 2
        -m_radius , -m_radius,  m_radius, // 7

        -m_radius , -m_radius, -m_radius, // 6
         m_radius , -m_radius, -m_radius, // 5
        -m_radius ,  m_radius, -m_radius, // 2
         m_radius ,  m_radius, -m_radius  // 1
    };

    glGenBuffers (1, &m_vboID);
    glBindBuffer (GL_ARRAY_BUFFER, m_vboID);
    glBufferData (GL_ARRAY_BUFFER, 3 * 14 * sizeof (float), &points, GL_STATIC_DRAW);

    glGenVertexArrays (1, &m_vaoID);
    glBindVertexArray (m_vaoID);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, m_vboID);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}


void Room::makeCubeMaps()
{
    glActiveTexture (GL_TEXTURE0);
    glGenTextures(1, &m_texID);

    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_POSITIVE_X, QString::fromStdString(RIGHT)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, QString::fromStdString(LEFT)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, QString::fromStdString(TOP)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, QString::fromStdString(BOTTOM)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, QString::fromStdString(BACK)));
    assert(loadTexture(m_texID, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, QString::fromStdString(FRONT)));

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glActiveTexture (GL_TEXTURE0);
    glGenTextures(1, &m_fakeID);

    setImages();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fakeID, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    generateProjections(.1f, 50.f);
}


void Room::bindFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}


void Room::render()
{
    glActiveTexture(GL_TEXTURE1);
    bindTexture();
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
    glBindVertexArray(0);
}


void Room::bindFakeTexture()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_fakeID);
}


void Room::bindTexture()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
}


void Room::setProjections(GLuint shader)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, "shadowMapProjections"), 6, GL_FALSE, glm::value_ptr(shadowMapProjections[0]));
}


void Room::setImages()
{
    GLenum side;
    QImage texture;
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_fakeID);

    for (int i = 0; i < 6; i++) {
        side = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        texture = m_images.value(side);
        glTexImage2D(side, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_images.value(side).bits());
    }
}


void Room::generateProjections(float zmin, float zmax) {

    glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.f, zmin, zmax);
    glm::mat4 view;

    float quarter = glm::radians(90.f);
    float half = glm::radians(180.f);

    glm::vec3 y_axis = glm::vec3(0.f, 1.f, 0.f);
    glm::vec3 x_axis = glm::vec3(1.f, 0.f, 0.f);

    view = glm::rotate(quarter, y_axis) *
            glm::rotate(half, x_axis);
//    gluLookAt(0.0, 0.0, 0.0,  1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // +X
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_POSITIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    view = glm::rotate(-quarter, y_axis) *
            glm::rotate(half, x_axis);
//    gluLookAt(0.0, 0.0, 0.0, -1.0, 0.0, 0.0,  0.0,-1.0, 0.0); // -X
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    view = glm::rotate(half, y_axis) *
            glm::rotate(quarter, x_axis);
//    gluLookAt(0.0, 0.0, 0.0,  0.0, 1.0, 0.0,  0.0, 0.0, 1.0); // +Y
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    view = glm::rotate(half, y_axis) *
            glm::rotate(-quarter, x_axis);
//    gluLookAt(0.0, 0.0, 0.0,  0.0,-1.0, 0.0,  0.0, 0.0,-1.0); // -Y
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    view = glm::rotate(half, x_axis);
//    gluLookAt(0.0, 0.0, 0.0,  0.0, 0.0, 1.0,  0.0,-1.0, 0.0); // +Z
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    view = glm::rotate(half, glm::vec3(0.f, 0.f, 1.f));
//    gluLookAt(0.0, 0.0, 0.0,  0.0, 0.0,-1.0,  0.0,-1.0, 0.0); // -Z
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;
}

bool Room::loadTexture(GLuint tex, GLenum side, const QString &filename)
{
    // make sure file exists
    QFile file(filename);
    if (!file.exists())
        return false;

    // load file into memory
    QImage image;
    image.load(file.fileName());
    image = image.mirrored(false, true);
    QImage texture = QGLWidget::convertToGLFormat(image);
    QImage textureSmall = QGLWidget::convertToGLFormat(image.scaled(cube_s, cube_s));

    m_images.insert(side, textureSmall);

    // make the texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

//    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    glTexImage2D(side, 0, GL_RGBA, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());

//    glTexImage2D( side, 0, GL_RGBA, cube_s, cube_s, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    return true;
}

