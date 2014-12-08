#ifndef ROOM_H
#define ROOM_H

#include "openglscene.h"

class Room
{
public:
    Room(float radius);
    virtual ~Room();

    void init();
    void makeCubeMap();

    void render();

private:
    bool loadTexture(GLuint tex, GLenum side, const QString &filename);

    GLuint m_vaoID; // The vertex array object ID, which is passed to glBindVertexArray.
    GLuint m_vboID;

    GLuint m_texID;

    float m_radius;
};

#endif // ROOM_H
