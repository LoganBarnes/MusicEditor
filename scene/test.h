#ifndef TEST_H
#define TEST_H

#include "openglscene.h"

class Test
{
public:
    Test();
    virtual ~Test();

    void init();
    void bindFramebuffer();
    void bindTexture();
    void setProjections(GLuint shader);

private:
    void generateProjections(float zmin, float zmax);

    GLuint m_texID;
    GLuint m_framebuffer;
    GLuint m_depthAttachment;

    glm::mat4 shadowMapProjections[6];
};

#endif // TEST_H
