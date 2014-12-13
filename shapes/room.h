#ifndef ROOM_H
#define ROOM_H

#include "openglscene.h"
#include <QHash>

class Room
{
public:
    Room(float radius);
    virtual ~Room();

    void init();
    QHash<GLenum, QImage> makeCubeMaps();

    void render();
    void bindFramebuffer();
    void bindFakeTexture();
    void bindTexture();
    void setModel(GLuint shader, glm::mat4 trans);
    void setImages();

private:
    bool loadTexture(GLuint tex, GLenum side, const QString &filename);

    GLuint m_vaoID; // The vertex array object ID, which is passed to glBindVertexArray.
    GLuint m_vboID;

    GLuint m_texID;
    GLuint m_fakeID;
    GLuint m_framebuffer;
    GLuint m_depthAttachment;

    QHash<GLenum, QImage> m_images;

    float m_radius;

    glm::mat4 shadowMapProjections[6];
};

#endif // ROOM_H
