#include "scene.h"
#include "room.h"
#include "grid.h"
#include "musicshape.h"
#include "udphandler.h"

glm::vec4 lightDirection = glm::normalize(glm::vec4(0.5f, -0.8f, 1.f, 0.f));

Scene::Scene(QObject *parent)
{
    m_global.ka = 1.f;
    m_global.kd = 1.f;
    m_global.ks = 1.f;
    m_global.kt = 1.f;

    // set shape pointer
    m_room = NULL;
    m_grid = NULL;
    m_lightningShape = NULL;
    m_waterShape = NULL;

    m_lights.clear();
    m_waterElements.clear();
    m_lightningElements.clear();
    m_waterLightningElements.clear();

    // music data
    m_udp1 = new UDPHandler(this, SLOT(setF1(QVector<float>)), 7000);
    m_udp1 = new UDPHandler(this, SLOT(setF2(QVector<float>)), 7000);
    m_udp1 = new UDPHandler(this, SLOT(setF3(QVector<float>)), 7000);

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


void Scene::setUp()
{
    CS123ScenePrimitive *prim = new CS123ScenePrimitive();
    CS123SceneMaterial& mat = prim->material;

    prim->meshfile = "";
  //  prim->type = PRIMITIVE_CUBE;

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
    light->type = LIGHT_DIRECTIONAL;
    light->dir = lightDirection;
    light->color.r = light->color.g = light->color.b = 1.f;
    light->id = 0;

    SceneElement *element = new SceneElement();
    element->primitive = prim;
    element->trans = glm::rotate(glm::mat4(), (float) (M_PI / 4.0), glm::vec3(1, 1, -.1f));
//    element->trans = glm::mat4();
//    element->inv = glm::mat4();
    element->inv = glm::inverse(element->trans);
    m_lights.append(light);
    m_lightningElements.append(element);

//    glm::mat4x4 rots = glm::rotate(glm::mat4(), (float) (M_PI / 4.0), glm::vec3(1, 1, -.1f));

    SceneElement *element2 = new SceneElement();
    element2->primitive = prim;
    element2->trans = glm::translate(glm::mat4(), glm::vec3(4, 0, 0));
    element2->inv = glm::inverse(element2->trans);

    SceneElement *element3 = new SceneElement();
    element3->primitive = prim;
    element3->trans = glm::translate(glm::mat4(), glm::vec3(-2, 0, 0));
    element3->inv = glm::inverse(element3->trans);

    m_waterElements.append(element2);
    m_waterElements.append(element3);


       // glUniform3f(glGetUniformLocation(m_shader, "allWhite"), 0, 0, 0); // not white
     //   m_shape->transformAndRender(m_shader, glm::translate(glm::mat4(), glm::vec3(2, 0, 0)));

    m_initialized = false;

}


void Scene::init()
{
    if (m_initialized)
        return;

    OpenGLScene::init(); // Call the superclass's init()

    m_room = new Room(5.f);
    m_room->init();
    m_room->makeCubeMap();

    m_grid = new Grid(5.f);
    m_grid->calcVerts();
    m_grid->updateGL(m_solidShader);
    m_grid->cleanUp();

    m_lightningShape = new MusicShape(150, 70, 0.15f);
    m_lightningShape->calcVerts();
    m_lightningShape->updateGL(m_solidShader);
    m_lightningShape->cleanUp();

    m_waterShape = new MusicShape(150, 70, 0.15f);
    m_waterShape->calcVerts();
    m_waterShape->updateGL(m_waterShader);
    m_waterShape->cleanUp();

    CS123SceneMaterial& mat = m_waterElements.at(0)->primitive->material;
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

    m_room->render();
}


void Scene::renderLightning()
{

    if (!m_initialized)
        return;

    applyMaterial(m_lightningElements.at(0)->primitive->material);

//    // Draw the grid.
    glUniform1i(m_solidUniforms["functionSize"], 0);
    glUniform3f(m_solidUniforms["allWhite"], 1, 1, 1); // make white
    m_grid->transformAndRender(m_solidShader, glm::mat4());


    // Draw the shapes.
    for (int i = 0; i < m_lightningElements.size(); ++i) {
        glUniform3f(glGetUniformLocation(m_solidShader, "allWhite"), 0, 0, 0); // not white
        glUniform1i(m_solidUniforms["functionSize"], m_f1.size());
        glUniform1fv(m_solidUniforms["function"], m_f1.size(), m_f1.data());
        m_lightningShape->transformAndRender(m_solidShader, m_lightningElements.at(i)->trans);

//        glUniform3f(glGetUniformLocation(m_shader, "allWhite"), 0, 0, 0); // not white
//        m_shape->transformAndRender(m_shader, glm::translate(glm::mat4(), glm::vec3(-2, 0, 0)));

//        glUniform3f(glGetUniformLocation(m_shader, "allWhite"), 0, 0, 0); // not white
//        m_shape->transformAndRender(m_shader, glm::translate(glm::mat4(), glm::vec3(2, 0, 0)));
    }
//    glUniform3f(m_solidUniforms["allWhite"], 0, 0, 0); // not white
//    m_solidShape->transformAndRender(m_solidShader, m_elements.at(0)->trans);

}


void Scene::renderTransparents()
{
    glUniform1f(m_waterUniforms["r0"], 0.25f);
    glUniform3f(m_waterUniforms["eta"], 1.f / 1.3312f, 1.f / 1.333f, 1.f / 1.3381);
    m_room->bindTexture();


    for (int i = 0; i < m_waterElements.size(); ++i) {
        glUniform1i(m_waterUniforms["functionSize"], m_f1.size());
        glUniform1fv(m_waterUniforms["function"], m_f1.size(), m_f1.data());
        m_waterShape->transformAndRender(m_waterShader, m_waterElements.at(i)->trans);


//        glUniform1i(m_waterUniforms["functionSize"], m_f2.size());
//        glUniform1fv(m_waterUniforms["function"], m_f2.size(), m_f2.data());
//        m_waterShape->transformAndRender(m_waterShader, glm::translate(
//                                        glm::rotate(glm::mat4(), (float) M_PI / 4.f, glm::vec3(1, 0, 0)),
//                                        glm::vec3(-2, 0, 0)));

//        glUniform1i(m_waterUniforms["functionSize"], m_f3.size());
//        glUniform1fv(m_waterUniforms["function"], m_f3.size(), m_f3.data());
//        m_waterShape->transformAndRender(m_waterShader, glm::translate(glm::mat4(), glm::vec3(2, 0, 0)));
    }
}


void Scene::setLights(const glm::mat4 viewMatrix)
{
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    CS123SceneLightData &light = *(m_lights.at(0));
    light.dir = glm::inverse(viewMatrix) * lightDirection;

    clearLights();
    setLight(light);
}

void Scene::updateShape(int ind, float x, float y, float z, PrimitiveType prm) {
    //std::cout << " ind  " << ind << " X " << x << " Y " << y << std::endl;
    if (prm == WATER_TYPE) {
        m_waterElements.at(ind)->trans = (glm::translate(glm::mat4(1.0f), glm::vec3(( 1.3f * x), (1.3f * y), z)) * m_waterElements.at(ind)->trans);
        m_waterElements.at(ind)->inv = glm::inverse(m_waterElements.at(ind)->trans);
    }
    else if (prm == LIGHTNING_TYPE) {
        m_lightningElements.at(ind)->trans = (glm::translate(glm::mat4(1.0f), glm::vec3(( 1.3f * x), (1.3f * y), z)) * m_lightningElements.at(ind)->trans);
        m_lightningElements.at(ind)->inv = glm::inverse(m_lightningElements.at(ind)->trans);
    }


//    glm::vec4 L = look; //gotten, passed in
//    glm::vec4 R = mRay; //gotten, passed in
//    glm::vec4 Iold = mHit; // passed in contact point
//    glm::vec4 Vold = Iold - eye; // pass in eye
//    glm::vec4 ANorm = glm::normalize(R);
//    glm::vec4 BNorm = glm::normalize(L);
//    float cosThet = glm::dot(ANorm, BNorm);
//    float VnewMag = (glm::dot(Vold, L)/cosThet);
//    glm::vec4 Inew = eye + (glm::normalize(R) * VnewMag);
//    glm::vec4 translaVec = Inew - Iold;

//    float tranSpeed = (delt/15.0f);
//    glm::mat4x4 transla = glm::mat4x4(1.0f, 0.0f, 0.0f, (translaVec.x),
//                                     0.0f, 1.0f, 0.0f, (translaVec.y),
//                                     0.0f, 0.0f, 1.0f, (translaVec.z),
//                                    0.0f, 0.0f, 0.0f, 1.0f);
//    glm::mat4x4 transfor = m_elements.at(ind)->trans;
//    m_elements.at(ind)->trans = (glm::transpose(transla) * transfor);
//   // m_hit = Inew;
}

IntersectElement Scene::shapeClickIntersect(glm::vec4 ey, glm::vec4 dr) {
    float finDist = -1.0f;
    int finInd = -1;
    PrimitiveType prm;
    for (int i = 0; i < m_waterElements.size(); ++i) {
        glm::vec4 eye = m_waterElements.at(i)->inv * ey;
        glm::vec4 dir = m_waterElements.at(i)->inv * dr;

        glm::vec3 eye3 = glm::vec3(eye);
        glm::vec3 dir3 = glm::vec3(dir);



        float tempDist = m_waterShape->calcIntersect(eye3, dir3);

        if (tempDist > 0.0f) {

            if(finDist < 0.0f) {
                finDist = tempDist;
                finInd = i;
                prm = WATER_TYPE;
            }
            else if (tempDist < finDist) {
                finDist = tempDist;
                finInd = i;
                prm = WATER_TYPE;
            }
        }
    }
    for (int i = 0; i < m_lightningElements.size(); ++i) {
        glm::vec4 eye = m_lightningElements.at(i)->inv * ey;
        glm::vec4 dir = m_lightningElements.at(i)->inv * dr;

        glm::vec3 eye3 = glm::vec3(eye);
        glm::vec3 dir3 = glm::vec3(dir);



        float tempDist = m_lightningShape->calcIntersect(eye3, dir3);

        if (tempDist > 0.0f) {

            if(finDist < 0.0f) {
                finDist = tempDist;
                finInd = i;
                prm = LIGHTNING_TYPE;
            }
            else if (tempDist < finDist) {
                finDist = tempDist;
                finInd = i;
                prm = LIGHTNING_TYPE;
            }
        }
    }

    IntersectElement intrRet = IntersectElement();
    intrRet.indx = finInd;
    intrRet.interT = finDist;
    intrRet.mHit = (ey + (dr * finDist));
    intrRet.prim = prm;
    return intrRet;
}


