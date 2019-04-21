#include "redoserver.h"

#include <QHash>
#include <QBuffer>
#include <QDataStream>
#include <QPointer>

class RedoServerPrivate
{
public:
    QString host;
    quint16 port;
    QTcpServer *server;
    QList<QTcpSocket*> sockets;
    QHash<QObject*, QPointer<QProcess>> processes;
    QStringList whitelist;
};

RedoServer::RedoServer(QObject *parent) : QObject(parent)
{
    p = new RedoServerPrivate;
    p->server = 0;
    p->port = 80;
}

RedoServer::~RedoServer()
{
    delete p;
}

void RedoServer::setHost(const QString &host)
{
    if(p->host == host)
        return;

    p->host = host;
    Q_EMIT hostChanged();
}

QString RedoServer::host()
{
    return p->host;
}

void RedoServer::setPort(quint16 port)
{
    if(p->port == port)
        return;

    p->port = port;
    Q_EMIT portChanged();
}

quint16 RedoServer::port()
{
    return p->port;
}

void RedoServer::setWhitelist(const QStringList &whitelist)
{
    if(p->whitelist == whitelist)
        return;

    p->whitelist = whitelist;
    Q_EMIT whitelistChanged();
}

QStringList RedoServer::whitelist()
{
    return whitelist();
}

void RedoServer::listen()
{
    if(p->host.isEmpty() || p->port<=0 || p->server)
        return;

    p->server = new QTcpServer(this);
    connect(p->server, &QTcpServer::newConnection, this, &RedoServer::newSocket);
    connect(p->server, &QTcpServer::newConnection, this, &RedoServer::newConnection);

    p->server->setMaxPendingConnections(500);
    if(!p->server->listen(QHostAddress(p->host), p->port)) {
        Q_EMIT failed();
    } else {
        Q_EMIT started();
    }
}

void RedoServer::newSocket()
{
    QTcpSocket *socket = p->server->nextPendingConnection();
    if(!p->whitelist.isEmpty()) {
        if(!p->whitelist.contains(socket->peerAddress().toString())) {
            socket->deleteLater();
            return;
        }
    }
    p->sockets.append(socket);

    connect(socket, &QTcpSocket::readyRead, this, &RedoServer::readData);
    connect(socket, &QTcpSocket::disconnected, this, &RedoServer::removeSocket);
}

void RedoServer::readData()
{
    QByteArray data = static_cast<QTcpSocket*>(QObject::sender())->readAll();
    RedoParser req(data);

    switch (req.getCode()) {
        case RedoParser::RunCommand:
            runCommand(QObject::sender(), req.getCommand());
            break;
        case RedoParser::InputChannel:

            break;
        case RedoParser::Termination:
            terminateProcess(QObject::sender());
            break;
    }
}

void RedoServer::sendResponse(QObject *receiver, RedoParser::ResponseType code, const QByteArray &response)
{
    RedoParser responseItem(code, response);
    QByteArray res = responseItem.writeRequest();

    static_cast<QTcpSocket*>(receiver)->write(res);
}

void RedoServer::removeSocket()
{
    QTcpSocket *senderSocket = static_cast<QTcpSocket*>(QObject::sender());
    p->sockets.removeOne(senderSocket);
    senderSocket->deleteLater();
}

void RedoServer::readyOutput(QObject *sender)
{
    QByteArray output = p->processes[sender]->readAllStandardOutput();
    sendResponse(sender, RedoParser::Output, output);
}

void RedoServer::readyError(QObject *sender)
{
    QByteArray error = p->processes[sender]->readAllStandardError();
    sendResponse(sender, RedoParser::Error, error);
}

void RedoServer::finished(QObject *sender, int exitCode, QProcess::ExitStatus exitStatus)
{
    sendResponse(sender, RedoParser::Finished, "");
}

void RedoServer::runCommand(QObject *sender, QByteArray command)
{
    QDataStream commandStream(&command, QIODevice::ReadOnly);
    QStringList args;
    commandStream >> args;
    QString program = args.takeFirst();

    QPointer<QProcess> process = new QProcess(sender);
    p->processes.insert(sender, process);

    process->start(program, args);

    connect(process, &QProcess::readyReadStandardOutput, this, [this, sender](){
        this->readyOutput(sender);
    });
    connect(process, &QProcess::readyReadStandardError, this, [this, sender](){
        this->readyError(sender);
    });
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [this, sender, process](int exitCode, QProcess::ExitStatus exitStatus){
        this->finished(sender, exitCode, exitStatus);

        process->deleteLater();
    });
}

void RedoServer::terminateProcess(QObject *sender)
{
    p->processes[sender]->close();
}
