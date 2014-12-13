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
    void setModel(GLuint shader, glm::vec4 eye);

private:
    bool loadTexture(GLuint tex, GLenum side, const QString &filename,
                     QHash<GLenum, QImage> images);

    GLuint m_vaoID; // The vertex array object ID, which is passed to glBindVertexArray.
    GLuint m_vboID;

    GLuint m_texID;

    float m_radius;
};

#endif // ROOM_H
