#ifndef OPENGLSCENE_H
#define OPENGLSCENE_H

#include <Common.h>
#include "CS123SceneData.h"
#include <QHash>


// Maximum number of lights, as defined in shader.
#define MAX_NUM_LIGHTS 10

class Sphere;
class Canvas;

//using namespace std;


struct SceneElement
{
    CS123ScenePrimitive *primitive;
    glm::mat4 trans;
    glm::mat4 inv;
};

/**
 * @class  OpenGLScene
 *
 * Basic Scene implementation that supports OpenGL. Students will extend this class in ShapesScene
 * and SceneviewScene.
 */
class OpenGLScene
{
public:
    OpenGLScene();
    virtual ~OpenGLScene();

    // Override this to do any initialization for the scene.
    virtual void init();

    // Render the scene.
    void render(Canvas *context);

    bool isInit();

protected:
    // Set all lights to black.
    void clearLights();

    // Set the light uniforms for the lights in the scene. (View matrix is used in cases where a
    // light follows the camera, as in ShapesScene.)
    virtual void setLights(const glm::mat4 viewMatrix) = 0;

    // Render geometry for Shapes and Sceneview.
    virtual void renderGeometry() = 0;

    // Set the necessary uniforms to switch materials.
    void applyMaterial(const CS123SceneMaterial &material);

    // Set the necessary uniforms for the light properties.
    void setLight(const CS123SceneLightData &light);

    // initialize the shapes with the given parameters
    void initShapes(int p1, int p2, float p3);

    // updates all the shape parameters
    void setShapeParams(int p1, int p2, float p3);

    // updates the current shape
    void updateShape(Sphere *shape);

    // updates all the shapes
    void updateShapes();

    // load textures
    int loadTexture(const QString &filename);

    // The program ID for OpenGL.
    GLuint m_shader;

//    Shape *m_cone;
//    Shape *m_cube;
//    Shape *m_cylinder;
    Sphere *m_water;
//    Shape *m_torus;

    bool m_initialized;
    bool m_initializedShapes;

    CS123SceneGlobalData m_global;
    QList<CS123SceneLightData*> m_lights;
    QList<SceneElement *> m_elements;

private:
    QHash<QString, GLint> m_uniformLocs;
};

#endif // OPENGLSCENE_H
