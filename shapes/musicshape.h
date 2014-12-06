#ifndef MUSICSHAPE_H
#define MUSICSHAPE_H

#include <QObject>
#include <QList>
#include "shape.h"
#include "udphandler.h"
#include "openglscene.h"

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
    virtual void make3Dslice(int *index, float thetaR, float thetaL);
    virtual void calcSliceSeg(int *index, float thetaR, float thetaL, float phi);

    float m_radius;

private:
    glm::vec2 f(float angle);

    GLuint m_shader;

    UDPHandler *m_udp;
    QList<float> m_function;
};

#endif // MUSICSHAPE_H
