#ifndef SCENE_H
#define SCENE_H

#include "openglscene.h"

class Room;
class Grid;
class Shape;
class UDPHandler;

class Scene : public QObject, public OpenGLScene
{

    Q_OBJECT
public:
    explicit Scene(QObject *parent = 0);
    virtual ~Scene();

    void setUp();
    virtual void init();

    virtual void sendMusicData(glm::vec4 eye);

    IntersectElement shapeClickIntersect(glm::vec4 ey, glm::vec4 dr);

    void updateShape(int ind, float x, float y, float z, PrimitiveType prm);
    void checkIntersects();

    void addObject(PrimitiveType typ);
    void deleteObject(PrimitiveType typ, int ind);



protected:
    // Set the light uniforms for the lights in the scene. (View matrix is used in cases where a
    // light follows the camera, as in ShapesScene.)
    virtual void setLights(const glm::mat4 viewMatrix, GLuint shader);

    // Render cubemap
    virtual void renderSetting();

    // Render geometry for Shapes and Sceneview.
    virtual void renderLightning(GLuint shader);

    void renderBolts();

    // Render see-through shapes
    virtual void renderTransparents(GLuint shader);

public slots:
    void setF1(QVector<float> f);
    void setF2(QVector<float> f);
    void setF3(QVector<float> f);
    void setF4(QVector<float> f);

private:
    bool m_initialized;

//    Room *m_room;
    Grid *m_grid;
    Shape *m_waterShape;
    Shape *m_lightningShape;
    Shape *m_waterLightningShape;


    UDPHandler *m_udp1;
    UDPHandler *m_udp2;
    UDPHandler *m_udp3;
    UDPHandler *m_udp4;

    QVector<float> m_f1;
    QVector<float> m_f2;
    QVector<float> m_f3;
    QVector<float> m_f4;
};

#endif // SCENE_H
