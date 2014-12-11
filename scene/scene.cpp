#include "scene.h"
#include "room.h"
#include "grid.h"
#include "musicshape.h"
#include "udphandler.h"

glm::vec4 lightDirection = glm::normalize(glm::vec4(0.5f, -0.8f, 1.f, 0.f));

Scene::Scene(QObject *parent)
{
    m_global.ka = 0.8f;
    m_global.kd = 0.8f;
    m_global.ks = 0.8f;
    m_global.kt = 1.f;

    // set shape pointer
    m_room = NULL;
    m_grid = NULL;
    m_solidShape = NULL;
    m_waterShape = NULL;

    m_lights.clear();
    m_elements.clear();

    // music data
    m_udp1 = new UDPHandler(this, SLOT(setF1(QVector<float>)), 7001);
    m_udp1 = new UDPHandler(this, SLOT(setF2(QVector<float>)), 7002);
    m_udp1 = new UDPHandler(this, SLOT(setF3(QVector<float>)), 7003);
    m_udp1 = new UDPHandler(this, SLOT(setF4(QVector<float>)), 7004);

    setUp();

    m_initialized = false;
}

Scene::~Scene()
{
}


void Scene::setF1(QVector<float> f)
{
    m_f1 = QVector<float>(f);
}


void Scene::setF2(QVector<float> f)
{
    m_f2 = QVector<float>(f);
}


void Scene::setF3(QVector<float> f)
{
    m_f3 = QVector<float>(f);
}


void Scene::setF4(QVector<float> f)
{
    m_f4 = QVector<float>(f);
}


void Scene::setUp()
{
    CS123ScenePrimitive *prim = new CS123ScenePrimitive();
    CS123SceneMaterial& mat = prim->material;

    prim->meshfile = "";
    prim->type = PRIMITIVE_CUBE;

    // Use a shiny orange material
    memset(&mat, 0, sizeof(CS123SceneMaterial));
    mat.cAmbient.r = 0.2f;
    mat.cAmbient.g = 0.2f;
    mat.cAmbient.b = 0.4f;
    mat.cDiffuse.r = 0.5f;
    mat.cDiffuse.g = 0.5f;
    mat.cDiffuse.b = 1.0f;
    mat.cSpecular.r = mat.cSpecular.g = mat.cSpecular.b = 1;
    mat.shininess = 64;

    // Use snow texture
    mat.textureMap = new CS123SceneFileMap();
//    mat.textureMap->filename = "/course/cs123/data/image/terrain/snow.JPG";
    mat.textureMap->filename = "/Users/Logan/Documents/course/cs123/data/image/terrain/snow.JPG";
    mat.textureMap->isUsed = false;
    mat.textureMap->repeatU = 1;
    mat.textureMap->repeatV = 1;
    mat.blend = 0.5f;

    mat.bumpMap = new CS123SceneFileMap();
    mat.bumpMap->filename = "";
    mat.bumpMap->isUsed = false;
    mat.bumpMap->repeatU = 1;
    mat.bumpMap->repeatV = 1;
    mat.bumpMap->texid = 0;

    CS123SceneLightData *light = new CS123SceneLightData();
    // Use a white directional light from the upper left corner
    memset(light, 0, sizeof(CS123SceneLightData));
    light->type = LIGHT_POINT;
//    light->pos = glm::vec4(-5.f, 8.f, -10.f, 1.f);
//    light->color.r = light->color.g = light->color.b = 1.f;
    light->pos = glm::vec4(-9.f, 0.f, 12.f, 1);
    light->color.r = 1.f;
    light->color.g = light->color.b = 0.5f;
    light->id = 0;

    SceneElement *element = new SceneElement();
    element->primitive = prim;
//    element->trans = glm::rotate(glm::mat4(), (float) (M_PI / 4.0), glm::vec3(1, 1, -.1f));
    element->trans = glm::translate(glm::vec3(0.f, 0.f, 0.f));
//    element->trans = glm::mat4();
//    element->inv = glm::mat4();
    element->inv = glm::inverse(element->trans);
    m_lights.append(light);
    m_elements.append(element);
}


void Scene::init()
{
    if (m_initialized)
        return;

    OpenGLScene::init(); // Call the superclass's init()

//    m_room = new Room(5.f);
//    m_room->init();
//    m_room->makeCubeMap();

    m_grid = new Grid(5.f);
    m_grid->calcVerts();
    m_grid->updateGL(m_solidShader);
    m_grid->cleanUp();

    m_solidShape = new MusicShape(100, 70, 0.15f);
    m_solidShape->calcVerts();
    m_solidShape->updateGL(m_solidShader);
    m_solidShape->cleanUp();

    m_waterShape = new MusicShape(150, 70, 0.15f);
    m_waterShape->calcVerts();
    m_waterShape->updateGL(m_waterShader);
    m_waterShape->cleanUp();

    CS123SceneMaterial& mat = m_elements.at(0)->primitive->material;
    int texId = loadTexture(QString::fromStdString(mat.textureMap->filename));
    if (texId == -1) {
        cout << "Texture '" << mat.textureMap->filename << "' does not exist" << endl;
        mat.textureMap->isUsed = 0;
    } else {
        mat.textureMap->texid = texId;
        mat.textureMap->isUsed = true;
    }

    m_initialized = true;
}


void Scene::renderSetting()
{

    if (!m_initialized)
        return;

//    m_room->render();
}


void Scene::renderSolids(GLuint shader)
{

    if (!m_initialized)
        return;

    applyMaterial(m_elements.at(0)->primitive->material, shader);

////     Draw the grid. CAN'T DRAW WITH CURRENT GEOMETRY SHADER SET TO TRIANGLES
//    glUniform1i(glGetUniformLocation(shader, "functionSize"), 0);
//    glUniform3f(glGetUniformLocation(shader, "allWhite"), 1, 1, 1); // make white
//    m_grid->transformAndRender(shader, glm::mat4());


    // Draw the shapes.
    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f2.size());
    glUniform1fv(glGetUniformLocation(shader, "function"), m_f2.size(), m_f2.data());
    glUniform3f(glGetUniformLocation(shader, "allWhite"), 0, 0, 0); // not white
//    m_solidShape->transformAndRender(shader, m_elements.at(0)->trans);
    m_solidShape->transformAndRender(shader, glm::translate(glm::vec3(0.f, 0.f, -2.f)));

    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f3.size());
    glUniform1fv(glGetUniformLocation(shader, "function"), m_f3.size(), m_f3.data());
    m_solidShape->transformAndRender(shader, glm::translate(glm::vec3(-2.f, 1.f, 0.f)));

    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f4.size());
    glUniform1fv(glGetUniformLocation(shader, "function"), m_f4.size(), m_f4.data());
    m_solidShape->transformAndRender(shader, glm::translate(glm::vec3(2.f, -1.f, 0.f)));
}


void Scene::renderTransparents(GLuint shader)
{

    glUniform1f(glGetUniformLocation(shader, "r0"), 0.1f);
    glUniform3f(glGetUniformLocation(shader, "eta"), 1.f / 1.3312f, 1.f / 1.333f, 1.f / 1.3381); // water
    glUniform1f(glGetUniformLocation(shader, "m"), 0.05f);
    glUniform4fv(glGetUniformLocation(shader, "lightPosition"), 1, glm::value_ptr(m_lights.value(0)->pos));
//    m_room->bindTexture();

    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f1.size());
    glUniform1fv(glGetUniformLocation(shader, "function"), m_f1.size(), m_f1.data());
    m_waterShape->transformAndRender(m_waterShader, m_elements.at(0)->trans);


//    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f2.size());
//    glUniform1fv(glGetUniformLocation(shader, "function"), m_f2.size(), m_f2.data());
//    m_waterShape->transformAndRender(m_waterShader, glm::translate(
//                                    glm::rotate(glm::mat4(), (float) M_PI / 4.f, glm::vec3(1, 0, 0)),
//                                    glm::vec3(-2, 0, 0)));

//    glUniform1i(glGetUniformLocation(shader, "functionSize"), m_f3.size());
//    glUniform1fv(glGetUniformLocation(shader, "function"), m_f3.size(), m_f3.data());
//    m_waterShape->transformAndRender(m_waterShader, glm::translate(glm::mat4(), glm::vec3(2, 0.f, .0f)));
}


void Scene::setLights(const glm::mat4 viewMatrix, GLuint shader)
{
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    CS123SceneLightData &light = *(m_lights.at(0));
//    light.dir = /*glm::inverse(viewMatrix) **/ lightDirection;

    clearLights(shader);
    setLight(light, shader);
}


