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
    void bindFramebuffer();
    void bindTexture();
    void setProjections(GLuint shader);

private:
    void generateProjections(float zmin, float zmax);
    bool loadTexture(GLuint tex, GLenum side, const QString &filename);

    GLuint m_vaoID; // The vertex array object ID, which is passed to glBindVertexArray.
    GLuint m_vboID;

    GLuint m_texID;
    GLuint m_framebuffer;
    GLuint m_depthAttachment;

    float m_radius;

    glm::mat4 shadowMapProjections[6];
};

#endif // ROOM_H
