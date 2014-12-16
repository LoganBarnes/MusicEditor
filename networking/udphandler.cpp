#include "udphandler.h"
//#include "scene.h"

#define PORT_SEND 7000

UDPHandler::UDPHandler(Scene *scene, const char *slot, int port, QObject *parent) :
    QObject(parent)
{
    m_port = port;
    m_socket = new QUdpSocket(this);
    m_socket->bind(QHostAddress::LocalHost, m_port);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(this, SIGNAL(sendFunction(QVector<float>)), scene, slot);
}


UDPHandler::~UDPHandler()
{
    delete m_socket;
}


void UDPHandler::setFunction(QVector<float> function)
{
//    cout << function.size() << endl;
    m_function = QVector<float>(function);
        cout << m_function.size() << endl;
}

int UDPHandler::getFunctionSize()
{
    return m_function.size();
}


QVector<float> UDPHandler::getFunction()
{
//    cout << m_function.size() << endl;
    return m_function;
}


void UDPHandler::sendInfo(float volume, float panning, bool electric)
{
    QByteArray data;
    data.append("port ");
    data.append(QByteArray::number(m_port));
    data.append(" volume ");
    data.append(QByteArray::number(volume));
    data.append(" ");
    data.append(QByteArray::number(panning));
    data.append(" ");
    data.append(QByteArray::number(electric));

    m_socket->writeDatagram(data.data(), QHostAddress::LocalHost, PORT_SEND);
}


void UDPHandler::sendAddition(int num)
{
    QByteArray data;
    data.append("add ");
    data.append(QByteArray::number(num));

    m_socket->writeDatagram(data.data(), QHostAddress::LocalHost, PORT_SEND);
}


void UDPHandler::sendFilter(float freq, float q)
{
    QByteArray data;
    data.append("port ");
    data.append(QByteArray::number(m_port));
    data.append(" filter ");
    data.append(QByteArray::number(freq));
    data.append(" ");
    data.append(QByteArray::number(q));

    m_socket->writeDatagram(data.data(), QHostAddress::LocalHost, PORT_SEND);
}


void UDPHandler::readyRead()
{
    QByteArray buffer;
    buffer.resize(m_socket->pendingDatagramSize());

    m_socket->readDatagram(buffer.data(), buffer.size());

    QString data(buffer);
    QStringList strings = data.split(" ");
    QList<float> floats;

    float f;
    bool converted;
    for (int i = 0; i < strings.length(); i++) {

        f = strings.value(i).toFloat(&converted);
        if (!converted)
            f = -96;
        floats.append(f);
    }
    QVector<float> function;
    /*
     * equation to map from one set of values to another:
     * (((val - oldMin) / (oldMax - oldMin)) * (newMax - newMin)) + newMin;
     */
    for (int i = 0; i < strings.length(); i++) {
        function.append(glm::max(0.f, (floats.value(i) + 96) / (120 * 1.5f)));
    }

   emit sendFunction(function);
}

