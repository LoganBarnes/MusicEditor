#ifndef SCENE_H
#define SCENE_H

#include "openglscene.h"

class Grid;
class Shape;

class Scene : public OpenGLScene
{
public:
    Scene();
    virtual ~Scene();

    virtual void init();

protected:
    // Set the light uniforms for the lights in the scene. (View matrix is used in cases where a
    // light follows the camera, as in ShapesScene.)
    virtual void setLights(const glm::mat4 viewMatrix);

    // Render geometry for Shapes and Sceneview.
    virtual void renderGeometry();

private:
    bool m_initialized;

    Grid *m_grid;
    Shape *m_shape;
};

#endif // SCENE_H
