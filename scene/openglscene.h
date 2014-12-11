#ifndef OPENGLSCENE_H
#define OPENGLSCENE_H

#include "Common.h"
#include "SceneData.h"
#include <map>


// Maximum number of lights, as defined in shader.
#define MAX_NUM_LIGHTS 10

class View;
class Camera;

using std::string;

struct SceneElement
{
    CS123ScenePrimitive *primitive;
    glm::mat4 trans;
    glm::mat4 inv;
    int link;
    bool render;
    bool linked;
    bool dragged;

};

struct IntersectElement
{
    float interT;
    int indx;
    float yMin;
    float yMax;
    float xMin;
    float xMax;
    PrimitiveType prim;
   // glm::vec4 mRay;
    glm::vec4 mHit;
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
    void render(Camera *cam);

    virtual IntersectElement shapeClickIntersect(glm::vec4 ey, glm::vec4 dr) = 0;
    virtual void updateShape(int ind, float x, float y, float z, PrimitiveType prm) = 0;

    virtual void addObject(PrimitiveType typ) = 0;
    virtual void deleteObject(PrimitiveType typ, int ind) = 0;

    virtual void checkIntersects() = 0;
    QList<SceneElement *> m_waterElements;
    QList<SceneElement *> m_lightningElements;
    QList<SceneElement *> m_deleteElements;


protected:
    // Set all lights to black.
    void clearLights();

    // Set the light uniforms for the lights in the scene. (View matrix is used in cases where a
    // light follows the camera, as in ShapesScene.)
    virtual void setLights(const glm::mat4 viewMatrix) = 0;

    // Render cubemap
    virtual void renderSetting() = 0;

    // Render solid geometry for Shapes and Sceneview.
    virtual void renderLightning() = 0;

    // Render see-through shapes
    virtual void renderTransparents() = 0;

    virtual void renderBolts() = 0;


    // Set the necessary uniforms to switch materials.
    void applyMaterial(const CS123SceneMaterial &material);

    // Set the necessary uniforms for the light properties.
    void setLight(const CS123SceneLightData &light);

    // Load a texture
    int loadTexture(const QString &filename);

    // The program ID for OpenGL.
    GLuint m_solidShader;
    GLuint m_cubeShader;
    GLuint m_waterShader;
    GLuint m_boltShader;

    CS123SceneGlobalData m_global;
    QList<CS123SceneLightData*> m_lights;



    std::map<string, GLint> m_solidUniforms;
    std::map<string, GLint> m_cubeUniforms;
    std::map<string, GLint> m_waterUniforms;
    std::map<string, GLint> m_boltUniforms;


private:

    bool m_drawWireframe;
    bool m_useLighting;
};

#endif // OPENGLSCENE_H
