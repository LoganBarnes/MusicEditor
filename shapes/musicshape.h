#ifndef MUSICSHAPE_H
#define MUSICSHAPE_H

#include <QObject>
#include <QList>
#include "shape.h"
#include "openglscene.h"

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

    virtual void transformAndRender(GLuint shader, glm::mat4 trans);

    float calcIntersect(glm::vec3 eye, glm::vec3 dir);

public slots:
    void setFunction(QVector<float> function);

protected:
    virtual void make3Dslice(int *index, float thetaR, float thetaL);
    virtual void calcSliceSeg(int *index, float thetaR, float thetaL, float phi);

    float m_radius;

private:
    glm::vec4 f(glm::vec3 *v, glm::vec3 *n);

    GLuint m_shader;

    UDPHandler *m_udp;
//    QVector<float> m_function;
};

#endif // MUSICSHAPE_H
