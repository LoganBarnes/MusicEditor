#ifndef VIEW_H
#define VIEW_H

#include "Common.h"
#include <qgl.h>
#include <QTime>
#include <QTimer>
#include "scene.h"

class OpenGLScene;
class Camera;

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QGLFormat format, QWidget *parent);
    ~View();

private:
    QTime time;
    QTimer timer;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    glm::vec3 filmPlaneTrans(glm::vec2 mouseMv);

    glm::vec2 calcBounds(int x, int y, float t);

    OpenGLScene *m_scene;
    Camera *m_camera;
    IntersectElement m_currMove;
    int m_oldX;
    int m_oldY;
    bool m_clicked;
    bool m_transZ;
    bool m_transLightningOut;
    bool m_delete;

    bool m_sceneChanged;

private slots:
    void tick();
};

#endif // VIEW_H

