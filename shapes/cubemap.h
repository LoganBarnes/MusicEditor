#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "openglscene.h"
#include <QHash>

class CubeMap
{
public:
    CubeMap();
    virtual ~CubeMap();

    void init();
    void makeCubeMap(QHash<GLenum, QImage> images);

    void bindFramebuffer();
    void bindTexture();
    void setProjections(GLuint shader, glm::mat4 trans);
    void setImages(QHash<GLenum, QImage> images);

private:

    GLuint m_vaoID; // The vertex array object ID, which is passed to glBindVertexArray.
    GLuint m_vboID;

    GLuint m_texID;
    GLuint m_framebuffer;
    GLuint m_depthAttachment;

    float m_radius;

    glm::mat4 shadowMapProjections[6];
};

#endif // CUBEMAP_H
