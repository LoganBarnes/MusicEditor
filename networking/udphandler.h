#ifndef UDPHANDLER_H
#define UDPHANDLER_H

#include <QObject>
#include <QUdpSocket>
#include <QList>
#include "scene.h"
//#include "musicshape.h"

//class MusicShape;

class UDPHandler : public QObject
{
    Q_OBJECT
public:
    UDPHandler(Scene *scene, const char *slot, int port, QObject *parent = 0);
    virtual ~UDPHandler();

    int getFunctionSize();
    QVector<float> getFunction();
    void sendInfo(float volume, float panning, bool electric);

signals:
    void sendFunction(QVector<float> function);

public slots:
    void readyRead();
    void setFunction(QVector<float> function);

private:
    QUdpSocket *m_socket;
    QVector<float> m_function;
    int m_port;

};

#endif // UDPHANDLER_H
