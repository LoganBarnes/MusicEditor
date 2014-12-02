#ifndef SCENE_H
#define SCENE_H

#include "openglscene.h"
#include "objects/Sphere.h"

/**
 *
 * @class Scene
 *
 * A scene that is used to render a single shape.
 *
 * This scene has no notion of the scene graph, so it will not be useful to you in
 * assignments requiring the display of multiple shapes. Luckily, the first time you
 * will need that kind of functionality is in the Sceneview assignment... and we've
 * left that task up to you in the SceneviewScene class.
 *
 * By default, the ShapesScene displays only a single triangle. You'll need to do
 * a little work here to render your shapes. You could render the shapes directly
 * in this class, or you could pass the work on to one or more subclasses. Think
 * carefully about your design here - you'll be reusing your shapes multiple times
 * during this course!
 */
class Scene : public OpenGLScene
{
public:
    Scene();
    virtual ~Scene();

    // Initialize the scene.
    virtual void init();
    virtual void update();
    virtual bool animate();
    void updateCurrentShape();

protected:
    // Set the light uniforms for the lights in the scene. (The view matrix is used so that the
    // light can follow the camera.)
    virtual void setLights(const glm::mat4 viewMatrix);

    // Render geometry for Shapes and Sceneview.
    virtual void renderGeometry();

private:
    void setShape();

    // Storage for private copies of the scene's light and material data. Note that these don't
    // need to be freed because they are VALUE types (not pointers) and the memory for them is
    // freed when the class itself is freed.
//    CS123SceneLightData  m_light;
//    CS123SceneMaterial   m_material;
//    CS123ScenePrimitive  m_prim;

    // stores all previous settings to check for changes
//    glm::vec4 m_oldSettings;

    // the current shape
    Sphere *m_sphere;

//    Shape *m_square;
//    Shape *m_ripplePlane;
//    Shape *m_rippleSphere;
//    Shape *m_music;
};

#endif // SCENE_H
