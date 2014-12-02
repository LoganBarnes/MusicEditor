#include "scene.h"
#include "camera.h"
#include <canvas.h>
#include <QFileDialog>
#include "objects/watersphere.h"

glm::vec4 lightDirection = glm::normalize(glm::vec4(1.f, -1.f, -1.f, 0.f));

Scene::Scene()
{
    m_global.ka = 1.f;
    m_global.kd = 1.f;
    m_global.ks = 1.f;
    m_global.kt = 1.f;

    CS123ScenePrimitive *prim = new CS123ScenePrimitive();
    CS123SceneMaterial& mat = prim->material;

    prim->meshfile = "";
    prim->type = PRIMITIVE_CUBE;

    // Use a shiny orange material
    memset(&mat, 0, sizeof(CS123SceneMaterial));
    mat.cAmbient.r = 0.1f;
    mat.cAmbient.g = 0.1f;
    mat.cAmbient.b = 0.2f;
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
    light->type = LIGHT_DIRECTIONAL;
    light->dir = lightDirection;
    light->color.r = light->color.g = light->color.b = 1.f;
    light->id = 0;

    // Store old settings and set shape pointer
    m_sphere = NULL;
//    m_oldSettings = glm::vec4(
//                settings.shapeType,
//                settings.shapeParameter1,
//                settings.shapeParameter2,
//                settings.shapeParameter3
//                );

    m_lights.clear();
    m_elements.clear();

    SceneElement *element = new SceneElement();
    element->primitive = prim;
    element->trans = glm::mat4();
    element->inv = glm::mat4();
    m_lights.append(light);
    m_elements.append(element);


    m_initialized = false;
}

Scene::~Scene()
{
//    if (m_square)
//        delete m_square;
//    if (m_ripplePlane)
//        delete m_ripplePlane;
//    if (m_rippleSphere)
//        delete m_rippleSphere;
//    if (m_music)
//        delete m_music;
    // Do not delete m_camera, it is owned by SupportScene3D

    // delete primitives

    delete m_sphere;
}


void Scene::updateCurrentShape() {
//    if (m_sphere != m_music)
        this->updateShape(m_sphere);
}

void Scene::init()
{
    if (m_initialized)
        return;

    OpenGLScene::init(); // Call the superclass's init()

//    this->initShapes(settings.shapeParameter1,
//                     settings.shapeParameter2,
//                     settings.shapeParameter3);
//    m_square = new Shape();
//    m_ripplePlane = new Ripple(settings.shapeParameter1,
//                               settings.shapeParameter2,
//                               0.5f);
//    m_rippleSphere = new RippleSphere(settings.shapeParameter1,
//                                      settings.shapeParameter2,
//                                      settings.shapeParameter3,
//                                      0.5f);
//    m_music = new MusicTracker(settings.shapeParameter1,
////    m_music = new MusicSphere(settings.shapeParameter1,
//                               settings.shapeParameter2,
//                               0.5f, m_shader, m_normalRenderer);

    updateShapes();
//    this->updateShape(m_square);
//    this->updateShape(m_ripplePlane);
//    this->updateShape(m_rippleSphere);
//    this->updateShape(m_music);

    this->setShape();

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


void Scene::setShape()
{
//    switch (settings.shapeType) {
//    case SHAPE_CUBE:
//        m_shape = m_cube;
//        m_elements.at(0)->primitive->type = PRIMITIVE_CUBE;
//        break;
//    case SHAPE_CONE:
//        m_shape = m_cone;
//        m_elements.at(0)->primitive->type = PRIMITIVE_CONE;
//        break;
//    case SHAPE_SPHERE:
//        m_shape = m_sphere;
//        m_elements.at(0)->primitive->type = PRIMITIVE_SPHERE;
//        break;
//    case SHAPE_CYLINDER:
//        m_shape = m_cylinder;
//        m_elements.at(0)->primitive->type = PRIMITIVE_CYLINDER;
//        break;
//    case SHAPE_TORUS:
//        m_shape = m_torus;
//        m_elements.at(0)->primitive->type = PRIMITIVE_TORUS;
//        break;
//    case SHAPE_SPECIAL_1:
////        m_shape = m_square;
////        m_elements.at(0)->primitive->type = PRIMITIVE_CUBE;
        m_sphere = m_water;
        m_elements.at(0)->primitive->type = PRIMITIVE_CUBE;
//        break;
//    case SHAPE_SPECIAL_2:
//        m_shape = m_ripplePlane;
//        m_elements.at(0)->primitive->type = PRIMITIVE_CUBE;
//        break;
//    case SHAPE_SPECIAL_3:
//        m_shape = m_rippleSphere;
//        m_elements.at(0)->primitive->type = PRIMITIVE_CUBE;
//        break;
//    default: // basic square shape
//        m_shape = m_square;
//        m_elements.at(0)->primitive->type = PRIMITIVE_CUBE;
//        break;
//    }
//    if (m_shape) {
//        m_shape->setParam1(settings.shapeParameter1);
//        m_shape->setParam2(settings.shapeParameter2);
//        m_shape->setParam3(settings.shapeParameter3);
//    }
}

/**
 * @brief Scene::update - checks for parameter or shape changes and updates
 * the appropriate objects.
 */
void Scene::update()
{
//    if (m_oldSettings[0] != settings.shapeType) {

//        m_oldSettings[0] = settings.shapeType;
//        setShape();
//        updateShape(m_sphere);
//    }
//    else if (m_oldSettings[1] != settings.shapeParameter1 &&
//             m_sphere->usesParam(1)) {

//        m_oldSettings[1] = settings.shapeParameter1;
//        m_sphere->setParam1(settings.shapeParameter1);
//        this->updateShape(m_sphere);

//    }
//    else if (m_oldSettings[2] != settings.shapeParameter2 &&
//               m_sphere->usesParam(2)) {

//        m_oldSettings[2] = settings.shapeParameter2;
//        m_sphere->setParam2(settings.shapeParameter2);
//        this->updateShape(m_sphere);

//    }
//    else if (m_oldSettings[3] != settings.shapeParameter3 &&
//               m_sphere->usesParam(3)) {

//        m_oldSettings[3] = settings.shapeParameter3;
//        m_sphere->setParam3(settings.shapeParameter3);
//        this->updateShape(m_sphere);
//    }
}

void Scene::renderGeometry()
{

    if (!m_initialized)
        return;

    applyMaterial(m_elements.at(0)->primitive->material);

    // Draw the shape.
    if (m_sphere)
        m_sphere->render();

}


bool Scene::animate()
{
    // return whether or not the shape is capable of animation
    if (m_sphere)
        return m_sphere->animate();
    return false;
}

void Scene::setLights(const glm::mat4 viewMatrix)
{
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    CS123SceneLightData &light = *(m_lights.at(0));
    light.dir = glm::inverse(viewMatrix) * lightDirection;

    clearLights();
    setLight(light);
}

