#include "udphandler.h"
#include "musicshape.h"

#define PORT_RECV 7000
#define PORT_SEND 7003

UDPHandler::UDPHandler(MusicShape *ms, QObject *parent) :
    QObject(parent)
{
    m_socket = new QUdpSocket(this);
    m_socket->bind(QHostAddress::LocalHost, PORT_RECV);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(this, SIGNAL(sendFunction(QList<float>)), ms, SLOT(setFunction(QList<float>)));
}


UDPHandler::~UDPHandler()
{
    delete m_socket;
}


void UDPHandler::requestData()
{
    QByteArray data;
    data.append("bang");

    m_socket->writeDatagram(data.data(), QHostAddress::LocalHost, PORT_SEND);
}


void UDPHandler::readyRead()
{
    QByteArray buffer;
    buffer.resize(m_socket->pendingDatagramSize());

    m_socket->readDatagram(buffer.data(), buffer.size());

    QString data(buffer);
    QStringList strings = data.split(" ");
    QList<float> function, floats;

    float oldMax = -96.f;
    float oldMin = 30.f;
    float f;
    for (int i = 0; i < strings.length(); i++) {
        f = strings.value(i).toFloat();
        floats.append(f);
        oldMin = min(oldMin, f);
        oldMax = max(oldMax, f);
    }
    /*
     * equation to map from one set of values to another:
     * (((val - oldMin) / (oldMax - oldMin)) * (newMax - newMin)) + newMin;
     */

//    float diff = oldMax - oldMin;
    for (int i = 0; i < strings.length(); i++) {
//        function.append((floats.value(i) - oldMin) / (diff * 1.5f));
        function.append(glm::max(0.f, (floats.value(i) + 96) / (120 * 1.5f)));
    }
    function.append(0.f);

   emit sendFunction(function);
}

