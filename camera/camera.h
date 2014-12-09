#ifndef CAMERA_H
#define CAMERA_H

#include "Common.h"

class Camera
{
public:
    Camera();

    // Called when the window size changes
    virtual void setAspectRatio(float);

    // C++ Note: The following are pure virtual functions - they must be
    // overridden in subclasses.

    // Return the projection matrix for the current camera settings.
    virtual glm::mat4x4 getProjectionMatrix() const;

    // Return the view matrix for the current camera settings.
    virtual glm::mat4x4 getViewMatrix() const;

    void updateMatrices();

    //get camera trans
    glm::mat4x4 getM3();
    glm::mat4x4 getM2();
    glm::mat4x4 getM4();

    glm::vec3 getEye();

    glm::vec3 getLook();



private:
    void updateProjectionMatrix();
    void updateViewMatrix();

    glm::mat4x4 m_viewMatrix;
    glm::mat4x4 m_projectionMatrix;
    float m_aspectRatio, m_angleX, m_angleY, m_zoomZ;
    int m_oldX, m_oldY;
};

#endif // CAMERA_H
