#ifndef SCENE_H
#define SCENE_H

#include "openglscene.h"
#include <QList>

#define DRUM_PORT 7001
#define PIANO_PORT 7002
#define LIVE_PORT 7003
#define SAMP_PORT1 7005
#define SAMP_PORT2 7006
#define SAMP_PORT3 7007
#define SAMP_PORT4 7008

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
    void checkAsserts(bool durClick);

    IntersectElement shapeClickIntersect(glm::vec4 ey, glm::vec4 dr);

    void updateShape(int ind, float x, float y, float z, PrimitiveType prm);
    void checkIntersects();

    void addObject(PrimitiveType typ);
    void deleteObject(PrimitiveType typ, int ind);

//    void checkFilters();



protected:
    // Set the light uniforms for the lights in the scene. (View matrix is used in cases where a
    // light follows the camera, as in ShapesScene.)
    virtual void setLights(const glm::mat4 viewMatrix, GLuint shader);

    // Render cubemap
    virtual void renderSetting();

    virtual void renderFilter(GLuint shader);

    // Render geometry for Shapes and Sceneview.
    virtual void renderLightning(GLuint shader);

    void renderBolts(GLuint shader);

    // Render see-through shapes
    virtual void renderTransparents(GLuint shader);

public slots:
    void setF1(QVector<float> f) { m_f1 = QVector<float>(f); }
    void setF2(QVector<float> f) { m_f2 = QVector<float>(f); }
    void setF3(QVector<float> f) { m_f3 = QVector<float>(f); }
    void setF5(QVector<float> f) { m_f5 = QVector<float>(f); }
    void setF6(QVector<float> f) { m_f6 = QVector<float>(f); }
    void setF7(QVector<float> f) { m_f7 = QVector<float>(f); }
    void setF8(QVector<float> f) { m_f8 = QVector<float>(f); }


private:
    bool m_initialized;

    Grid *m_grid;
    Shape *m_waterShape;
    Shape *m_lightningShape;
    Shape *m_waterLightningShape;
    Shape *m_filter;

    CS123SceneMaterial *m_lightningMaterial;

    QList<int> m_unused;

    UDPHandler *m_udp1;
    UDPHandler *m_udp2;
    UDPHandler *m_udp3;
    UDPHandler *m_udp5;
    UDPHandler *m_udp6;
    UDPHandler *m_udp7;
    UDPHandler *m_udp8;

    QVector<float> m_f1;
    QVector<float> m_f2;
    QVector<float> m_f3;
    QVector<float> m_f5;
    QVector<float> m_f6;
    QVector<float> m_f7;
    QVector<float> m_f8;
};

#endif // SCENE_H
