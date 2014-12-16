#ifndef FILTERSHAPE_H
#define FILTERSHAPE_H

#include "shape.h"

class FilterShape : public Shape
{
public:
    FilterShape(int p1, int p2, float p3, float radius);
    virtual ~FilterShape();

    virtual void calcVerts();

protected:
    virtual void make3Dslice(int *index, float phiL, float phiR, bool first, bool last, float leftU, float rightU);

    float m_radius;
    float m_r;
    float m_R;
};

#endif // FILTERSHAPE_H
