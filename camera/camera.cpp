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

void Camera::swing()
{
    m_angleY++;
    updateViewMatrix();
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
//    m_projectionMatrix = glm::perspective(
//                glm::radians(90.f), 1.f, 0.1f, 50.f);
}

void Camera::updateViewMatrix()
{
    m_viewMatrix =
            glm::translate(glm::vec3(0.f, -0.0f, m_zoomZ)) *
            glm::rotate(glm::radians(0.f), glm::vec3(1.f, 0.f, 0.f)) *
            glm::rotate(glm::radians(m_angleY), glm::vec3(0.f, 1.f, 0.f));
}
