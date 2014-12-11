#ifndef OPENGLSCENE_H
#define OPENGLSCENE_H

#include "Common.h"
#include "SceneData.h"
#include <map>


// Maximum number of lights, as defined in shader.
#define MAX_NUM_LIGHTS 10

class View;
class Camera;
class Room;
class Test;

using std::string;

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
    void render(Camera *cam, bool test);

protected:
    // Set all lights to black.
    void clearLights(GLuint shader);

    // Set the light uniforms for the lights in the scene. (View matrix is used in cases where a
    // light follows the camera, as in ShapesScene.)
    virtual void setLights(const glm::mat4 viewMatrix, GLuint shader) = 0;

    // Render cubemap
    virtual void renderSetting() = 0;

    // Render solid geometry for Shapes and Sceneview.
    virtual void renderSolids(GLuint shader) = 0;

    // Render see-through shapes
    virtual void renderTransparents(GLuint shader) = 0;

    // Set the necessary uniforms to switch materials.
    void applyMaterial(const CS123SceneMaterial &material, GLuint shader);

    // Set the necessary uniforms for the light properties.
    void setLight(const CS123SceneLightData &light, GLuint shader);

    // Load a texture
    int loadTexture(const QString &filename);

//    Test *m_test;
    Room *m_room;

    // The program ID for OpenGL.
    GLuint m_solidShader;
    GLuint m_cubeShader;
    GLuint m_testShader;
    GLuint m_waterShader;

    CS123SceneGlobalData m_global;
    QList<CS123SceneLightData*> m_lights;
    QList<SceneElement *> m_elements;

private:

    std::map<string, GLint> m_solidUniforms;
    std::map<string, GLint> m_cubeUniforms;
    std::map<string, GLint> m_waterUniforms;

    bool m_drawWireframe;
    bool m_useLighting;
};

#endif // OPENGLSCENE_H
