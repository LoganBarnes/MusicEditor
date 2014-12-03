#ifndef MUSICSHAPE_H
#define MUSICSHAPE_H

#include <QObject>
#include <QList>
#include "shape.h"

class UDPHandler;

class MusicShape : public QObject, public Shape
{
    Q_OBJECT
public:
    explicit MusicShape(int p1, int p2, float radius,
                          GLuint shader, QObject *parent = 0);
    virtual ~MusicShape();

    virtual void calcVerts();
    virtual bool animate();

public slots:
    void setFunction(QList<float> function);

protected:
    float m_radius;

private:
    glm::vec3 calcNormal(glm::vec3 p);
    glm::vec4 f(float x, float y);
    glm::vec4 fv(glm::vec2 v);
    float df(float x, float y);

    GLuint m_shader;

    int m_rate;
    UDPHandler *m_udp;
    QList<float> m_function;
};

#endif // MUSICSHAPE_H
