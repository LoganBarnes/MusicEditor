#include "view.h"
#include "scene.h"
#include "camera.h"
#include <QApplication>
#include <QKeyEvent>
#include <iostream>

using namespace std;

const int cube_s = 1024;

View::View(QGLFormat format, QWidget *parent) : QGLWidget(format, parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
//    setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    m_camera = new Camera();
    m_oldX = 0;
    m_oldY = 0;
    m_clicked = false;
    m_transZ = false;
    m_transLightningOut = false;
    m_delete = false;
    m_rotate = false;

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    m_sceneChanged = true;
}

View::~View()
{
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    glGetError(); // Clear errors after call to glewInit
    if (GLEW_OK != err)
    {
        // Problem: glewInit failed, something is seriously wrong.
        fprintf(stderr, "Error initializing glew: %s\n", glewGetErrorString(err));
    }

    // Initialize scene
    m_scene = new Scene();
    m_scene->init();

    // Enable depth testing, so that objects are occluded based on depth instead of drawing order.
    glEnable(GL_DEPTH_TEST);

    // Move the polygons back a bit so lines are still drawn even though they are coplanar with the
    // polygons they came from, which will be drawn before them.
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1, -1);

    // Enable back-face culling, meaning only the front side of every face is rendered.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Specify that the front face is represented by vertices in counterclockwise order (this is
    // the default).
    glFrontFace(GL_CCW);

    m_camera->updateMatrices();

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    time.start();
    timer.start(1000 / 20); // 20 fps

    // Center the mouse, which is explained more in mouseMoveEvent() below.
    // This needs to be done here because the mouse may be initially outside
    // the fullscreen window and will not automatically receive mouse move
    // events. This occurs if there are two monitors and the mouse is on the
    // secondary monitor.
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

//    std::printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));
}

void View::paintGL()
{

    // TODO: Implement the demo rendering here
    if (m_scene != NULL)
    {
        // Check for errors from the last frame.
        int err;
        if ((err = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr, "GL is in an error state before painting.\n");
            fprintf(stderr, "(GL error code %d)\n", err);
            fprintf(stderr, "%s\n", glewGetErrorString(err));
        }

        // Update the scene camera.
        if (m_sceneChanged) {
            glViewport(0, 0, cube_s, cube_s);
            m_scene->setCubeMaps(m_camera); // set cube map
            m_sceneChanged = false;
        }

        glViewport(0, 0, width(), height());
        m_camera->setAspectRatio((float)width() / (float)height());

        // Render the scene.
        m_scene->render(m_camera);
        if (!m_transLightningOut) {
            m_scene->checkIntersects();
        }
    }
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
    m_scene->checkAsserts(false);

    if ((event->x() > 0 && event->x() < width()) && (event->y() > 0 && event->y() < height())) {

        int x = event->x();
        int y = event->y();
        glm::vec4 camPos = glm::inverse(m_camera->getViewMatrix()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        float xPos = ((2.0f * x)/width()) - 1.0f;
        float yPos = 1.0f - ((2.0f * y)/height());
        glm::vec4 filmPos = glm::vec4(xPos, yPos, -1.0f, 1.0f);


        glm::vec4 fWorld = (glm::inverse(m_camera->getViewMatrix()) * glm::inverse(m_camera->getM2()) * filmPos);


        glm::vec4 direc = glm::normalize((fWorld - camPos));


        m_currMove = m_scene->shapeClickIntersect(camPos, direc);
        if (m_currMove.indx < 0) {
            return;
        }
        bool wLight = false;
        if (m_currMove.prim == WATER_TYPE) {
            if (m_scene->m_waterElements.at(m_currMove.indx)->linked) {
                wLight = true;
                int lInd = m_scene->m_waterElements.at(m_currMove.indx)->link;
                if (m_transLightningOut) {
                    int wInd = m_currMove.indx;
                    m_currMove.indx = lInd;
                    m_currMove.prim = LIGHTNING_TYPE;
                    m_scene->m_lightningElements.at(lInd)->render = true;
                    m_scene->m_waterElements.at(wInd)->link = -1;
                    m_scene->m_waterElements.at(wInd)->linked = false;
                }
            }
        }

        glm::vec3 ht = glm::vec3(camPos + (direc * m_currMove.interT));
        m_currMove.mHit = glm::vec4(ht, 1.0f);





        if (m_delete) {
            if (wLight) {
                int lInd = m_scene->m_waterElements.at(m_currMove.indx)->link;
                int wInd = m_currMove.indx;
                m_scene->m_lightningElements.at(lInd)->render = true;
                m_scene->m_waterElements.at(wInd)->link = -1;
                m_scene->m_waterElements.at(wInd)->linked = false;
            }
            m_scene->deleteObject(m_currMove.prim, m_currMove.indx);
            return;
        }
        m_clicked = true;


        if (m_currMove.prim == WATER_TYPE) {
            m_scene->m_waterElements.at(m_currMove.indx)->dragged = true;
        }
        else if (m_currMove.prim == LIGHTNING_TYPE) {
            m_scene->m_lightningElements.at(m_currMove.indx)->dragged = true;
        }

//        if (m_currMove.indx >= 0) {
//            m_clicked = true;
//            m_currMove.xMax = calcBounds(width(), yPos, m_currMove.interT).x;
//            m_currMove.xMin = calcBounds(0, yPos, m_currMove.interT).x;
//            m_currMove.yMax = calcBounds(xPos, 0, m_currMove.interT).y;
//            m_currMove.yMin = calcBounds(xPos, height(), m_currMove.interT).y;
//        }
    }
}

//glm::vec2 View::calcBounds(int x, int y, float tVal) {
////    glm::vec4 camPos = glm::inverse(m_camera->getViewMatrix()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
////    float xPos = ((2.0f * x)/width()) - 1.0f;
////    float yPos = 1.0f - ((2.0f * y)/height());
////    glm::vec4 filmPos = glm::vec4(xPos, yPos, -1.0f, 1.0f);
////    glm::vec4 fWorld = (glm::inverse(m_camera->getM4()) * glm::inverse(m_camera->getM3()) * glm::inverse(m_camera->getM2()) * filmPos);
////    glm::vec4 direc = glm::normalize((fWorld - camPos));
////    //std::cout << " direc " << std::to_string(direc) << "  world"
////    return glm::vec2(camPos + (tVal * direc));
//    return glm::vec2(0.0f, 0.0f);

//}

glm::vec3 View::filmPlaneTrans(glm::vec2 mouseMv) {

    int x = mouseMv.x;
    int y = mouseMv.y;
    glm::vec4 camPos = glm::inverse(m_camera->getViewMatrix()) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    float xPos = ((2.0f * x)/width()) - 1.0f;
    float yPos = 1.0f - ((2.0f * y)/height());
    glm::vec4 filmPos = glm::vec4(xPos, yPos, -1.0f, 1.0f);


    glm::vec4 fWorld = (glm::inverse(m_camera->getViewMatrix()) * glm::inverse(m_camera->getM2()) * filmPos);


    glm::vec4 direc = glm::normalize((fWorld - camPos));



    glm::vec4 L = glm::vec4(m_camera->getLook(), 0.0f);

    glm::vec3 rI = glm::vec3(direc);


    glm::vec4 R = glm::vec4(rI, 0.0f);
    glm::vec4 Iold = m_currMove.mHit;
    glm::vec4 Vold = glm::vec4(glm::vec3((Iold - m_camera->getEye4())), 0.0f);

    glm::vec4 ANorm = R;
    glm::vec4 BNorm = L;
    float cosThet = glm::dot(ANorm, BNorm);
    float VnewMag = (glm::dot(Vold, L)/cosThet);

    glm::vec4 Inew = (glm::vec4(glm::vec3(m_camera->getEye4()), 1.0f) + (R * VnewMag));


    glm::vec4 translaVec = (Inew - Iold);
    m_currMove.mHit = Inew;
    return glm::vec3(translaVec);
}

void View::mouseMoveEvent(QMouseEvent *event)
{


//    int deltX = event->x() - m_oldX;
    int deltY = ((event->y() - m_oldY) *  -1);
    glm::vec3 translaVec = glm::vec3(0.0f, 0.0f, 0.0f);
    if (m_clicked) {
        if (m_transZ) {
            glm::vec4 look = glm::vec4(m_camera->getLook(), 0.0f);
            float tranSpeed = (deltY/15.0f);
            translaVec = glm::vec3((look * tranSpeed));
        }
        else {
            translaVec = filmPlaneTrans(glm::vec2(event->x(), event->y()));
        }
    }


    if ((event->x() > 0 && event->x() < width()) && (event->y() > 0 && event->y() < height())) {
        if (m_clicked) {
            m_scene->checkAsserts(true);
            m_sceneChanged = true;
//            float xTot = (m_currMove.xMax - m_currMove.xMin);
//            float yTot = (m_currMove.yMax - m_currMove.yMin);
//            float xRat = ((1.0f * width()) / xTot);
//            float yRat = ((1.0f * height()) / yTot);
//            if ((isinf(xRat) == 1) || xRat == 0.0f) {
//                xRat = 1.0f;
//                deltX = 0.0f;
//            }

//            int lInd = -1;
//            if (m_currMove.prim == WATER_TYPE) {
//                if (m_scene->m_waterElements.at(m_currMove.indx)->linked) {
//                    lInd = m_scene->m_waterElements.at(m_currMove.indx)->link;
//                }
//            }
            if (m_transZ) {
//                if (m_transLightningOut && (lInd >= 0)) {
//                    m_scene->updateShape(lInd, translaVec.x, translaVec.y, translaVec.z, LIGHTNING_TYPE);
//                    m_scene->m_waterElements.at(m_currMove.indx)->linked = false;
//                    m_scene->m_waterElements.at(m_currMove.indx)->link = -1;
//                    m_scene->m_lightningElements.at(lInd)->render = true;
//                    m_currMove.indx = lInd;
//                    m_currMove.prim = LIGHTNING_TYPE;

//                }
//                else {
                    m_scene->updateShape(m_currMove.indx, translaVec.x, translaVec.y, translaVec.z, m_currMove.prim);
//                }

            }
            else {
//                if (m_transLightningOut && (lInd >= 0)) {
//                    m_scene->updateShape(lInd, translaVec.x, translaVec.y, translaVec.z, LIGHTNING_TYPE);
//                    m_scene->m_waterElements.at(m_currMove.indx)->linked = false;
//                    m_scene->m_waterElements.at(m_currMove.indx)->link = -1;
//                    m_scene->m_lightningElements.at(lInd)->render = true;
//                    m_currMove.indx = lInd;
//                    m_currMove.prim = LIGHTNING_TYPE;
//                }
//                else {
                    m_scene->updateShape(m_currMove.indx, translaVec.x, translaVec.y, translaVec.z, m_currMove.prim);
//                }
            }

            if (m_currMove.prim == WATER_TYPE) {
                if (m_scene->m_waterElements.at(m_currMove.indx)->linked) {
                    int lInd = m_scene->m_waterElements.at(m_currMove.indx)->link;
                    m_scene->m_lightningElements.at(lInd)->trans = m_scene->m_waterElements.at(m_currMove.indx)->trans;
                }
            }

        }
        m_oldX = event->x();
        m_oldY = event->y();
    }
    else {
        std::cout << " IN DRAGGED OUT " << std::endl;
        if (m_clicked) {
            m_clicked = false;
            if (m_currMove.prim == WATER_TYPE) {
                m_scene->m_waterElements.at(m_currMove.indx)->dragged = false;
            }
            else if (m_currMove.prim == LIGHTNING_TYPE) {
                m_scene->m_lightningElements.at(m_currMove.indx)->dragged = false;
            }
        }
    }

}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_clicked) {
        if (m_currMove.prim == WATER_TYPE) {
            m_scene->m_waterElements.at(m_currMove.indx)->dragged = false;
        }
        else if (m_currMove.prim == LIGHTNING_TYPE) {
            m_scene->m_lightningElements.at(m_currMove.indx)->dragged = false;
        }
    }
    m_clicked = false;

    m_scene->checkAsserts(false);

}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here
    if (event->key() == Qt::Key_Shift) {
        m_transZ = true;
        m_transLightningOut = false;
        m_delete = false;

    }
    if (event->key() == Qt::Key_Control) {
        m_transLightningOut = true;
        m_transZ = false;
        m_delete = false;
    }
    if (event->key() == Qt::Key_R) {
        if (m_rotate) {
            m_rotate = false;
        }
        else {
            m_rotate = true;
        }
        m_transLightningOut = false;
        m_transZ = false;
        m_delete = false;
    }

    if (event->key() == Qt::Key_D) {
        m_delete = true;
        m_transLightningOut = false;
        m_transZ = false;
    }
    if (event->key() == Qt::Key_L) {
        m_scene->addObject(LIGHTNING_TYPE);
    }
    if (event->key() == Qt::Key_W) {
        m_scene->addObject(WATER_TYPE);
    }
}

void View::keyReleaseEvent(QKeyEvent *event)
{
    m_transZ = false;
    m_transLightningOut = false;
    m_delete = false;

}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
//    float seconds = time.restart() * 0.001f;

    // TODO: Implement the demo update here
    if (m_rotate) {
        m_camera->swing();
    }

    m_scene->sendMusicData(m_camera->getEye4());
    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
