#include "openglscene.h"
#include "camera.h"
#include "ResourceLoader.h"
#include "view.h"
#include <string>
#include <sstream>
#include "QCoreApplication"
#include <QFile>
#include "room.h"

#define SHAPE_RADIUS 0.5f

OpenGLScene::OpenGLScene()
{
    m_drawWireframe = false;
    m_useLighting = true;
}

OpenGLScene::~OpenGLScene()
{
    // delete primitives
    int i;
    int num = m_waterElements.size();
    for (i = 0; i < num; i++)
    {
        SceneElement *e = m_waterElements.at(i);
        delete e->primitive->material.bumpMap;
        delete e->primitive->material.textureMap;
        delete e->primitive;
        delete e;
    }
    m_waterElements.clear();

    num = m_lightningElements.size();
    for (i = 0; i < num; i++)
    {
        SceneElement *e = m_lightningElements.at(i);
        delete e->primitive->material.bumpMap;
        delete e->primitive->material.textureMap;
        delete e->primitive;
        delete e;
    }
    m_lightningElements.clear();

    num = m_deleteElements.size();
    for (i = 0; i < num; i++)
    {
        SceneElement *e = m_deleteElements.at(i);
        delete e->primitive->material.bumpMap;
        delete e->primitive->material.textureMap;
        delete e->primitive;
        delete e;
    }
    m_deleteElements.clear();


    // delete lights
    num = m_lights.size();
    for (i = 0; i < num; i++)
    {
        CS123SceneLightData *l = m_lights.at(i);
        delete l;
    }
    m_lights.clear();

    delete m_room;
}


void OpenGLScene::init()
{
    m_solidShader = ResourceLoader::loadShadersWithGeom(
                ":/shaders/default.vert",
                ":/shaders/default.gsh",
                ":/shaders/default.frag");
    m_solidCubeShader = ResourceLoader::loadShadersWithGeom(
                ":/shaders/default.vert",
                ":/shaders/cube.gsh",
                ":/shaders/default.frag");
    m_cubeShader = ResourceLoader::loadShaders(
                ":/shaders/cube.vert",
                ":/shaders/cube.frag");
    m_waterShader = ResourceLoader::loadShaders(
                ":/shaders/water.vert",
                ":/shaders/water.frag");
    m_boltShader = ResourceLoader::loadShadersWithGeom(
                ":/shaders/bolt.vert",
                ":/shaders/bolt.gsh",
                ":/shaders/bolt.frag");
    m_boltCubeShader = ResourceLoader::loadShadersWithGeom(
                ":/shaders/bolt.vert",
                ":/shaders/boltcube.gsh",
                ":/shaders/bolt.frag");
//    m_orbShader = ResourceLoader::loadShaders(
//                ":/shaders/orb.vert",
//                ":/shaders/orb.frag");

    generateProjections(.1f, 50.f);

    m_room = new Room(25.f);
    m_room->init();
    m_images = m_room->makeCubeMaps();
}


void OpenGLScene::render(Camera *cam)
{
    // Clear the screen in preparation for the next frame. (Use a gray background instead of a
    // black one for drawing wireframe or normals so they will show up against the background.)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the view matrix from the camera
    assert(cam);
    glm::mat4 viewMatrix = cam->getViewMatrix();
    glm::mat4 projMatrix = cam->getProjectionMatrix();

    // cubemap
    GLuint shader = m_cubeShader;
    glUseProgram(shader);
    glDepthMask(GL_FALSE);

    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
            glm::value_ptr(projMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
                       glm::value_ptr(viewMatrix));
    m_room->setModel(shader, cam->getEye4());
    glUniform1i(glGetUniformLocation(shader, "envMap"), 1);

    m_room->render();

    glDepthMask(GL_TRUE);

    // solids
    shader = m_solidShader;
    glUseProgram(shader);

    // Set scene uniforms.
    clearLights(shader);
    setLights(viewMatrix, shader);
    glUniform1i(glGetUniformLocation(shader, "useLighting"), m_useLighting);
    glUniform1i(glGetUniformLocation(shader, "useArrowOffsets"), GL_FALSE);
    glUniform3fv(glGetUniformLocation(shader, "eyeV"), 1, glm::value_ptr(cam->getEye()));
    glUniform1f(glGetUniformLocation(shader, "rad"), 0.15f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
            glm::value_ptr(projMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
            glm::value_ptr(viewMatrix));
    glUniform3f(glGetUniformLocation(shader, "allBlack"), 1, 1, 1);

    renderLightning(shader);

    // lighting bolts
    shader = m_boltShader;
    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
            glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
            glm::value_ptr(projMatrix));

    renderBolts();

    // water
    shader = m_waterShader;
    glUseProgram(shader);

    // water stuffs
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projMatrix));
    glUniform1i(glGetUniformLocation(shader, "envMap"), 1);

    renderTransparents(shader);

    glUseProgram(0);
}

void OpenGLScene::setCubeMaps(Camera *cam)
{
    // Clear the screen in preparation for the next frame. (Use a gray background instead of a
    // black one for drawing wireframe or normals so they will show up against the background.)
//    m_room->bindFramebuffer();

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the view matrix from the camera
    assert(cam);
    glm::mat4 viewMatrix = cam->getViewMatrix();
    glm::mat4 projMatrix = cam->getProjectionMatrix();

    // solids
    GLuint shader;
    shader = m_solidCubeShader;
    glUseProgram(shader);
//        m_room->setImages();
//        m_room->bindFakeTexture();
    glUniformMatrix4fv(glGetUniformLocation(shader, "shadowMapProjections"), 6, GL_FALSE, glm::value_ptr(shadowMapProjections[0]));
//        m_room->setModel(shader, m_waterElements.value(0)->trans);

    // Set scene uniforms.
    clearLights(shader);
    setLights(viewMatrix, shader);
    glUniform1i(glGetUniformLocation(shader, "useLighting"), m_useLighting);
    glUniform1i(glGetUniformLocation(shader, "useArrowOffsets"), GL_FALSE);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
            glm::value_ptr(projMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
            glm::value_ptr(viewMatrix));
    glUniform3f(glGetUniformLocation(shader, "allBlack"), 1, 1, 1);

    renderLightning(shader);


    shader = m_boltCubeShader;
    glUseProgram(shader);
//        m_room->setModel(shader, m_waterElements.value(0)->trans);

    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE,
            glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE,
            glm::value_ptr(projMatrix));

    renderBolts();
    glUseProgram(0);
}


void OpenGLScene::applyMaterial(const CS123SceneMaterial &material, GLuint shader)
{
    // Make sure the members of CS123SceneColor are packed tightly
    COMPILE_TIME_ASSERT(sizeof(CS123SceneColor) == sizeof(float) * 4);

    glUniform3fv(glGetUniformLocation(shader, "ambient_color"), 1, &material.cAmbient.r);
    glUniform3fv(glGetUniformLocation(shader, "diffuse_color"), 1, &material.cDiffuse.r);
    glUniform3fv(glGetUniformLocation(shader, "specular_color"), 1, &material.cSpecular.r);
    glUniform1f(glGetUniformLocation(shader, "shininess"), material.shininess);

    if (material.textureMap && material.textureMap->isUsed && material.textureMap->texid) {
        glUniform1i(glGetUniformLocation(shader, "useTexture"), 1);
        glUniform1i(glGetUniformLocation(shader, "tex"), 1);
        glUniform1f(glGetUniformLocation(shader, "repeatU"), material.textureMap->repeatU);
        glUniform1f(glGetUniformLocation(shader, "repeatV"), material.textureMap->repeatV);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.textureMap->texid);
        glActiveTexture(GL_TEXTURE0);
    } else {
        glUniform1i(glGetUniformLocation(shader, "useTexture"), 0);
    }
}

void OpenGLScene::clearLights(GLuint shader)
{
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        glUniform3f(glGetUniformLocation(shader, ("lightColors" + indexString).c_str()), 0, 0, 0);
    }
}

void OpenGLScene::setLight(const CS123SceneLightData &light, GLuint shader)
{
    std::ostringstream os;
    os << light.id;
    std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.

    bool ignoreLight = false;

    GLint lightType;
    switch(light.type)
    {
    case LIGHT_POINT:
        lightType = 0;
        glUniform3fv(glGetUniformLocation(shader, ("lightPositions" + indexString).c_str()), 1,
                glm::value_ptr(light.pos));
        break;
    case LIGHT_DIRECTIONAL:
        lightType = 1;
        glUniform3fv(glGetUniformLocation(shader, ("lightDirections" + indexString).c_str()), 1,
                glm::value_ptr(glm::normalize(light.dir)));
        break;
    default:
        lightType = -1;
        ignoreLight = true; // Light type not supported
        break;
    }

    CS123SceneColor color = light.color;
    if (ignoreLight) color.r = color.g = color.b = 0;

    glUniform1i(glGetUniformLocation(shader, ("lightTypes" + indexString).c_str()), lightType);
    glUniform3f(glGetUniformLocation(shader, ("lightColors" + indexString).c_str()),
                color.r, color.g, color.b);
    glUniform3f(glGetUniformLocation(shader, ("lightAttenuations" + indexString).c_str()),
            light.function.x, light.function.y, light.function.z);
}


// Copied from lab04
int OpenGLScene::loadTexture(const QString &filename)
{
    // make sure file exists
    QFile file(filename);
    if (!file.exists())
        return -1;

    // load file into memory
    QImage image;
    image.load(file.fileName());
    image = image.mirrored(false, true);
    QImage texture = QGLWidget::convertToGLFormat(image);

    // generate texture ID
    GLuint id = 0;
    glGenTextures(1, &id);

    // make the texture
    glBindTexture(GL_TEXTURE_2D, id);

    // copy image data into texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());

    // filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // coordinate wrapping options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return id;
}


void OpenGLScene::generateProjections(float zmin, float zmax) {

    glm::mat4 proj = glm::perspective(glm::radians(90.f), 1.f, zmin, zmax);
    glm::mat4 view;

    float quarter = glm::radians(90.f);
    float half = glm::radians(180.f);

    glm::vec3 y_axis = glm::vec3(0.f, 1.f, 0.f);
    glm::vec3 x_axis = glm::vec3(1.f, 0.f, 0.f);

    // +X
    view = glm::rotate(quarter, y_axis) *
            glm::rotate(half, x_axis);
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_POSITIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    // -X
    view = glm::rotate(-quarter, y_axis) *
            glm::rotate(half, x_axis);
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    // +Y
    view = glm::rotate(-quarter, x_axis);
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    // -Y
    view = glm::rotate(quarter, x_axis);
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    // +Z
    view = glm::rotate(half, x_axis);
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;

    // -Z
    view = glm::rotate(half, glm::vec3(0.f, 0.f, 1.f));
    shadowMapProjections[
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z - GL_TEXTURE_CUBE_MAP_POSITIVE_X
    ] = proj * view;
}
