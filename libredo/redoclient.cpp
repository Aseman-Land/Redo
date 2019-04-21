#include "redoclient.h"

#include <QBuffer>
#include <QDataStream>

class RedoClientPrivate
{
public:
    QString host;
    quint16 port;
    QTcpSocket *socket;
};

RedoClient::RedoClient(QObject *parent) : QObject(parent)
{
    p = new RedoClientPrivate;
    p->port = 80;
    p->socket = 0;
}

RedoClient::~RedoClient()
{
    delete p;
}

void RedoClient::setHost(const QString &host)
{
    if(p->host == host)
        return;

    p->host = host;
    Q_EMIT hostChanged();
}

QString RedoClient::host()
{
    return p->host;
}

void RedoClient::setPort(quint16 port)
{
    if(p->port == port)
        return;

    p->port = port;
    Q_EMIT portChanged();
}

quint16 RedoClient::port()
{
    return p->port;
}

void RedoClient::connect()
{
    if(p->host.isEmpty() || p->port<=0 || p->socket)
        return;

    p->socket = new QTcpSocket(this);

    QObject::connect(p->socket, &QTcpSocket::connected, this, [this](){
        Q_EMIT connected();
    });
    QObject::connect(p->socket, &QTcpSocket::readyRead, this, &RedoClient::readData);

    p->socket->connectToHost(p->host, p->port);
    if(!p->socket->waitForConnected(5000)) {
        Q_EMIT failed();
    }
}

void RedoClient::readData()
{
    QByteArray data = static_cast<QTcpSocket*>(QObject::sender())->readAll();
    RedoParser response(data);

    switch (response.getCode()) {
        case RedoParser::Output:
            Q_EMIT readyOutput(response.getCommand());
            break;
        case RedoParser::Error:
            Q_EMIT readyError(response.getCommand());
            break;
        case RedoParser::Finished:
            Q_EMIT finished();
            break;
    }
}

void RedoClient::sendRequest(RedoParser::RequestType code)
{
    RedoParser req(code, "");
    QByteArray request = req.writeRequest();

    p->socket->write(request);
}

void RedoClient::sendRequest(RedoParser::RequestType code, const QByteArray &command)
{
    RedoParser req(code, command);
    QByteArray request = req.writeRequest();

    p->socket->write(request);
}
