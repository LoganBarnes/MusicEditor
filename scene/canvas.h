#ifndef SCENE_H
#define SCENE_H

#include "GL/glew.h"
#include <QGLWidget>

class Canvas : public QGLWidget
{
    Q_OBJECT
public:
    Canvas(QGLFormat format, QWidget *parent);
    virtual ~Canvas();
};

#endif // SCENE_H
