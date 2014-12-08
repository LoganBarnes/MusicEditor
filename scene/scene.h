#ifndef SCENE_H
#define SCENE_H

#include "openglscene.h"

class Shape;

class Scene : public OpenGLScene
{
public:
    Scene();
    virtual ~Scene();

    virtual void init();


    int shapeClickIntersect(glm::vec4 ey, glm::vec4 dr);

protected:
    // Set the light uniforms for the lights in the scene. (View matrix is used in cases where a
    // light follows the camera, as in ShapesScene.)
    virtual void setLights(const glm::mat4 viewMatrix);

    // Render geometry for Shapes and Sceneview.
    virtual void renderGeometry();

private:
    bool m_initialized;

    Shape *m_shape;
};

#endif // SCENE_H
