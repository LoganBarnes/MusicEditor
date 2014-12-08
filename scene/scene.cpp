#include "scene.h"
#include "musicshape.h"

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
    m_shape = NULL;

    m_lights.clear();
    m_elements.clear();

    SceneElement *element = new SceneElement();
    element->primitive = prim;
    element->trans = glm::rotate(glm::mat4(), (float) (M_PI / 4.0), glm::vec3(1, 1, -.1f));
//    element->trans = glm::mat4();
    element->inv = glm::mat4();
    m_lights.append(light);
    m_elements.append(element);

    m_initialized = false;
}

Scene::~Scene()
{
}

void Scene::init()
{
    if (m_initialized)
        return;

    OpenGLScene::init(); // Call the superclass's init()

    m_shape = new MusicShape(25, 25, 0.15f, m_shader);
    m_shape->calcVerts();
    m_shape->updateGL(m_shader);
    m_shape->cleanUp();

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


void Scene::renderGeometry()
{

    if (!m_initialized)
        return;

    applyMaterial(m_elements.at(0)->primitive->material);

    // Draw the shape.
    if (m_shape)
        m_shape->transformAndRender(m_shader, m_elements.at(0)->trans);

}


void Scene::setLights(const glm::mat4 viewMatrix)
{
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    CS123SceneLightData &light = *(m_lights.at(0));
    light.dir = glm::inverse(viewMatrix) * lightDirection;

    clearLights();
    setLight(light);
}

int shapeClickIntersect(glm::vec4 ey, glm::vec4 dr) {
    for (int i = 0; i < m_sceneRef; ++i) {
        if (i != currInd) {
            glm::vec4 eye = m_inv[i] * ey;
            glm::vec4 dir = m_inv[i] * dr;

            glm::vec3 eye3 = glm::vec3(eye);
            glm::vec3 dir3 = glm::vec3(dir);

            CS123RayIntersect tempInt;

            if (m_sceneRef->m_shapes->at(i).type == PRIMITIVE_SPHERE) {
                tempInt = m_impSphere->calcIntersect(eye3, dir3);
            }
            else if (m_sceneRef->m_shapes->at(i).type == PRIMITIVE_CUBE) {
                tempInt = m_impCube->calcIntersect(eye3, dir3);
            }
            else if (m_sceneRef->m_shapes->at(i).type == PRIMITIVE_CYLINDER) {
                tempInt = m_impCylinder->calcIntersect(eye3, dir3);
            }
            else if (m_sceneRef->m_shapes->at(i).type == PRIMITIVE_CONE) {
                tempInt = m_impCone->calcIntersect(eye3, dir3);
            }

            if (tempInt.t > 0.0f) {
                if(currIntersect.t < 0.0f) {
                    currIntersect = tempInt;
                    currIntersect.shapeInd = i;
                }
                else if (tempInt.t < currIntersect.t) {
                    currIntersect = tempInt;
                    currIntersect.shapeInd = i;
                }
            }
        }
    }

    if (currIntersect.t > 0.0f) {
        return illuminate(ey, dr, currIntersect, recurs);

    }
    else {

        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
}


