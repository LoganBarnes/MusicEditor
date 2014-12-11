#include "openglscene.h"
#include "camera.h"
#include "ResourceLoader.h"
#include "view.h"
#include <string>
#include <sstream>
#include "QCoreApplication"
#include <QFile>

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
}


void OpenGLScene::init()
{
    m_solidShader = ResourceLoader::loadShaders(
            ":/shaders/default.vert",
            ":/shaders/default.frag");
    m_cubeShader = ResourceLoader::loadShaders(
                ":/shaders/cube.vert",
                ":/shaders/cube.frag");
    m_waterShader = ResourceLoader::loadShaders(
                ":/shaders/glass.vert",
                ":/shaders/glass.frag");
    m_boltShader = ResourceLoader::loadShaders(
                ":/shaders/bolt.vert",
                ":/shaders/bolt.frag");

    // solids
    m_solidUniforms["projection"]= glGetUniformLocation(m_solidShader, "projection");
    m_solidUniforms["view"]= glGetUniformLocation(m_solidShader, "view");
    m_solidUniforms["model"]= glGetUniformLocation(m_solidShader, "model");
    m_solidUniforms["ambient_color"] = glGetUniformLocation(m_solidShader, "ambient_color");
    m_solidUniforms["diffuse_color"] = glGetUniformLocation(m_solidShader, "diffuse_color");
    m_solidUniforms["specular_color"] = glGetUniformLocation(m_solidShader, "specular_color");
    m_solidUniforms["shininess"] = glGetUniformLocation(m_solidShader, "shininess");
    m_solidUniforms["repeatU"] = glGetUniformLocation(m_solidShader, "repeatU");
    m_solidUniforms["repeatV"] = glGetUniformLocation(m_solidShader, "repeatV");
    m_solidUniforms["useLighting"]= glGetUniformLocation(m_solidShader, "useLighting");
    m_solidUniforms["useArrowOffsets"] = glGetUniformLocation(m_solidShader, "useArrowOffsets");
    m_solidUniforms["allBlack"]= glGetUniformLocation(m_solidShader, "allBlack");
    m_solidUniforms["allWhite"]= glGetUniformLocation(m_solidShader, "allWhite");
    m_solidUniforms["functionSize"]= glGetUniformLocation(m_solidShader, "functionSize");
    m_solidUniforms["function"]= glGetUniformLocation(m_solidShader, "function");
    m_solidUniforms["tex"] = glGetUniformLocation(m_solidShader, "tex");
    m_solidUniforms["useTexture"] = glGetUniformLocation(m_solidShader, "useTexture");

    // cube
    m_cubeUniforms["projection"] = glGetUniformLocation(m_cubeShader, "projection");
    m_cubeUniforms["view"] = glGetUniformLocation(m_cubeShader, "view");
    m_cubeUniforms["envMap"] = glGetUniformLocation(m_cubeShader, "envMap");

    // water
    m_waterUniforms["view"] = glGetUniformLocation(m_waterShader, "view");
    m_waterUniforms["projection"] = glGetUniformLocation(m_waterShader, "projection");
    m_waterUniforms["model"] = glGetUniformLocation(m_waterShader, "model");
    m_waterUniforms["functionSize"] = glGetUniformLocation(m_waterShader, "functionSize");
    m_waterUniforms["function"] = glGetUniformLocation(m_waterShader, "function");
    m_waterUniforms["r0"] = glGetUniformLocation(m_waterShader, "r0");
    m_waterUniforms["eta"] = glGetUniformLocation(m_waterShader, "eta");

    // bolt
    m_boltUniforms["view"] = glGetUniformLocation(m_boltShader, "view");
    m_boltUniforms["projection"] = glGetUniformLocation(m_boltShader, "projection");
    m_boltUniforms["model"] = glGetUniformLocation(m_boltShader, "model");

}

void OpenGLScene::render(Camera *cam)
{
    // Clear the screen in preparation for the next frame. (Use a gray background instead of a
    // black one for drawing wireframe or normals so they will show up against the background.)
    if (m_drawWireframe) glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
    else glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get the view matrix from the camera
    assert(cam);
    glm::mat4 viewMatrix = cam->getViewMatrix();
    glm::mat4 projMatrix = cam->getProjectionMatrix();

    // cubemap
    glUseProgram(m_cubeShader);
    glDepthMask(GL_FALSE);

    glUniformMatrix4fv(glGetUniformLocation(m_cubeShader, "projection"), 1, GL_FALSE,
            glm::value_ptr(projMatrix));
    glUniformMatrix4fv(glGetUniformLocation(m_cubeShader, "view"), 1, GL_FALSE,
                       glm::value_ptr(viewMatrix));
//    glUniformMatrix4fv(glGetUniformLocation(m_cubeShader, "view"), 1, GL_FALSE,
//                       glm::value_ptr(glm::rotate(viewMatrix, (float) M_PI, glm::vec3(0, 1, 0))));
    glUniform1i(glGetUniformLocation(m_cubeShader, "envMap"), 1);

    renderSetting();

    glDepthMask(GL_TRUE);

    // solids
    glUseProgram(m_solidShader);

    // Set scene uniforms.
    clearLights();
    setLights(viewMatrix);
    glUniform1i(m_solidUniforms["useLighting"], m_useLighting);
    glUniform1i(m_solidUniforms["useArrowOffsets"], GL_FALSE);
    glUniformMatrix4fv(m_solidUniforms["projection"], 1, GL_FALSE,
            glm::value_ptr(projMatrix));
    glUniformMatrix4fv(m_solidUniforms["view"], 1, GL_FALSE,
            glm::value_ptr(viewMatrix));
    glUniform3f(m_solidUniforms["allBlack"], 1, 1, 1);

    renderLightning();


    glUseProgram(m_boltShader);
    glUniformMatrix4fv(m_boltUniforms["view"], 1, GL_FALSE,
            glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(m_boltUniforms["projection"], 1, GL_FALSE,
            glm::value_ptr(projMatrix));

    renderBolts();

//    if (m_drawWireframe)
//    {
//        glUniform3f(m_solidUniforms["allBlack"], 0, 0, 0);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//        renderSolids();

//        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    }

    // water
    glUseProgram(m_waterShader);

    // water stuffs
    glUniformMatrix4fv(m_waterUniforms["view"], 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(m_waterUniforms["projection"], 1, GL_FALSE, glm::value_ptr(projMatrix));
    glUniform1i(m_waterUniforms["envMap"], 1);

    renderTransparents();

    glUseProgram(0);
}


void OpenGLScene::applyMaterial(const CS123SceneMaterial &material)
{
    // Make sure the members of CS123SceneColor are packed tightly
    COMPILE_TIME_ASSERT(sizeof(CS123SceneColor) == sizeof(float) * 4);

    glUniform3fv(m_solidUniforms["ambient_color"], 1, &material.cAmbient.r);
    glUniform3fv(m_solidUniforms["diffuse_color"], 1, &material.cDiffuse.r);
    glUniform3fv(m_solidUniforms["specular_color"], 1, &material.cSpecular.r);
    glUniform1f(m_solidUniforms["shininess"], material.shininess);

    if (material.textureMap && material.textureMap->isUsed && material.textureMap->texid) {
        glUniform1i(m_solidUniforms["useTexture"], 1);
        glUniform1i(m_solidUniforms["tex"], 1);
        glUniform1f(m_solidUniforms["repeatU"], material.textureMap->repeatU);
        glUniform1f(m_solidUniforms["repeatV"], material.textureMap->repeatV);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.textureMap->texid);
        glActiveTexture(GL_TEXTURE0);
    } else {
        glUniform1i(m_solidUniforms["useTexture"], 0);
    }
}

void OpenGLScene::clearLights()
{
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        glUniform3f(glGetUniformLocation(m_solidShader, ("lightColors" + indexString).c_str()), 0, 0, 0);
    }
}

void OpenGLScene::setLight(const CS123SceneLightData &light)
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
        glUniform3fv(glGetUniformLocation(m_solidShader, ("lightPositions" + indexString).c_str()), 1,
                glm::value_ptr(light.pos));
        break;
    case LIGHT_DIRECTIONAL:
        lightType = 1;
        glUniform3fv(glGetUniformLocation(m_solidShader, ("lightDirections" + indexString).c_str()), 1,
                glm::value_ptr(glm::normalize(light.dir)));
        break;
    default:
        lightType = -1;
        ignoreLight = true; // Light type not supported
        break;
    }

    CS123SceneColor color = light.color;
    if (ignoreLight) color.r = color.g = color.b = 0;

    glUniform1i(glGetUniformLocation(m_solidShader, ("lightTypes" + indexString).c_str()), lightType);
    glUniform3f(glGetUniformLocation(m_solidShader, ("lightColors" + indexString).c_str()),
                color.r, color.g, color.b);
    glUniform3f(glGetUniformLocation(m_solidShader, ("lightAttenuations" + indexString).c_str()),
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
