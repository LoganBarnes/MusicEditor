#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "openglscene.h"
#include <QHash>

class CubeMap
{
public:
    CubeMap();
    virtual ~CubeMap();

    void makeCubeMap(QHash<GLenum, QImage> images);

    void bindFramebuffer();
    void bindTexture();
    void setModel(GLuint shader, glm::mat4 trans);
    void setImages(QHash<GLenum, QImage> images);

private:

    GLuint m_texID;
    GLuint m_framebuffer;
    GLuint m_depthAttachment;
};

#endif // CUBEMAP_H
