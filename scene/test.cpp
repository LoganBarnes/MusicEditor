#include "test.h"

const int cube_s = 1024;

Test::Test()
{
}


void Test::init()
{
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glActiveTexture (GL_TEXTURE0);
    glGenTextures(1, &m_texID);

    for (int i = 0; i < 6; i++) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, cube_s, cube_s, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texID, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    generateProjections(.1f, 50.f);
}

Test::~Test()
{
}


void Test::setProjections(GLuint shader)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, "shadowMapProjections"), 6, GL_FALSE, glm::value_ptr(shadowMapProjections[0]));
}


void Test::bindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texID, 0);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthAttachment);
}


void Test::bindTexture() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
}

void Test::generateProjections(float zmin, float zmax) {

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
