#include "view.h"
#include "scene.h"
#include "camera.h"
#include <QApplication>
#include <QKeyEvent>
#include <iostream>

using namespace std;

View::View(QGLFormat format, QWidget *parent) : QGLWidget(format, parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
//    setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    m_camera = new Camera();

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

//        m_scene->sendMu

        // Update the scene camera.
        if (m_sceneChanged) {
            glViewport(0, 0, 128, 128);
            m_scene->render(m_camera, true); // set cube map
            m_sceneChanged = false;
        }

        glViewport(0, 0, width(), height());
        m_camera->setAspectRatio((float)width() / (float)height());

        // Render the scene.
        m_scene->render(m_camera, false);
    }
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    int deltaX = event->x() - width() / 2;
    int deltaY = event->y() - height() / 2;
    if (!deltaX && !deltaY) return;
//    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    // TODO: Handle mouse movements here
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here
}

void View::keyReleaseEvent(QKeyEvent *event)
{
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
//    float seconds = time.restart() * 0.001f;

    // TODO: Implement the demo update here
    m_camera->swing();

    m_scene->sendMusicData(m_camera->getEye());
    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
