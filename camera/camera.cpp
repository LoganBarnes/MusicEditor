#include "camera.h"

Camera::Camera()
{
    m_aspectRatio = 1;
    m_angleX = m_angleY = 0;
    m_zoomZ = -5;
}

void Camera::setAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;

    updateProjectionMatrix();
}

glm::mat4x4 Camera::getProjectionMatrix() const
{
    return m_projectionMatrix;
}

glm::mat4x4 Camera::getViewMatrix() const
{
    return m_viewMatrix;
}

void Camera::updateMatrices()
{
    updateProjectionMatrix();
    updateViewMatrix();
}

void Camera::updateProjectionMatrix()
{
    // Make sure glm gets a far value that is greater than the near value.
    // Thanks Windows for making far a keyword!

    m_projectionMatrix = glm::perspective(
            glm::radians(55.f), m_aspectRatio, 0.1f, 50.f);
}

void Camera::updateViewMatrix()
{
    m_viewMatrix =
            glm::translate(glm::vec3(0.f, 0.f, m_zoomZ)) *
            glm::rotate(glm::radians(m_angleY), glm::vec3(0.f, 1.f, 0.f)) *
            glm::rotate(glm::radians(m_angleX), glm::vec3(1.f, 0.f, 0.f));
}


glm::mat4x4 Camera::getM2() {
    //Aspect ratio = m_aspectRatio
    //Far PLane = 50.0f;
    //Height Angle = 55.0f
    float hRad = glm::radians(55.0f);
    float temH = glm::tan(hRad/2.0f);
    float temW = m_aspectRatio * temH;
    float cotH = 1.0f/temH;
    float cotW = 1.0f/temW;
    glm::mat4x4 m2 = glm::mat4x4(
       cotW/50.0f, 0, 0, 0,
       0, cotH/50.0f, 0, 0,
       0, 0, 1/50.0f, 0,
       0, 0, 0, 1
    );

    return glm::transpose(m2);
}

glm::mat4x4 Camera::getM3() {
    glm::mat4x4 idMat = glm::mat4x4(1.0f);

    glm::mat4x4 firstRot = glm::rotate(idMat, glm::radians(m_angleY), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4x4 rotMat = glm::rotate(firstRot, glm::radians(m_angleX), glm::vec3(1.f, 0.f, 0.f));

//    glm::mat4x4  m3 = glm::mat4x4(
//       m_u.x, m_u.y, m_u.z, 0,
//       m_v.x, m_v.y, m_v.z, 0,
//       m_w.x, m_w.y, m_w.z, 0,
//       0, 0, 0, 1
//    );

//    return glm::transpose(m3);
    return rotMat;
}

glm::mat4x4 Camera::getM4() {
    glm::mat4x4  m4 = glm::mat4x4(
       1, 0, 0, 0,
       0, 1, 0, 0,
       0, 0, 1, m_zoomZ,
       0, 0, 0, 1
    );

    return glm::transpose(m4);
}
