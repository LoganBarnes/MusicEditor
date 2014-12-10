#ifndef MUSICSHAPE_H
#define MUSICSHAPE_H

#include <QObject>
#include <QList>
#include "shape.h"
#include "openglscene.h"

class MusicShape : public QObject, public Shape
{

    Q_OBJECT
public:
    explicit MusicShape(int p1, int p2, float radius, QObject *parent = 0);
    virtual ~MusicShape();

    virtual void calcVerts();
    virtual bool animate();

    virtual void transformAndRender(GLuint shader, glm::mat4 trans);

    float calcIntersect(glm::vec3 eye, glm::vec3 dir);

public slots:
    //void setFunction(QVector<float> function);


protected:
    virtual void make3Dslice(int *index, float thetaR, float thetaL);
    virtual void calcSliceSeg(int *index, float thetaR, float thetaL, float phi);

    float m_radius;

};

#endif // MUSICSHAPE_H
