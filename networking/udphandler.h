#ifndef UDPHANDLER_H
#define UDPHANDLER_H

#include <QObject>
#include <QUdpSocket>
#include <QList>

class MusicShape;

class UDPHandler : public QObject
{
    Q_OBJECT
public:
    UDPHandler(MusicShape *ms, QObject *parent = 0);
    virtual ~UDPHandler();

    void requestData();

signals:
    void sendFunction(QList<float> function);

public slots:
    void readyRead();

private:
    QUdpSocket *m_socket;

};

#endif // UDPHANDLER_H
