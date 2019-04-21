#ifndef REDOSERVER_H
#define REDOSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QProcess>

#include "redoparser.h"

class RedoServerPrivate;
class RedoServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QStringList whitelist READ whitelist WRITE setWhitelist NOTIFY whitelistChanged)

public:
    RedoServer(QObject *parent = nullptr);
    virtual ~RedoServer();

    void setHost(const QString &host);
    QString host();

    void setPort(quint16 port);
    quint16 port();

    void setWhitelist(const QStringList &whitelist);
    QStringList whitelist();

Q_SIGNALS:
    void hostChanged();
    void portChanged();
    void whitelistChanged();
    void started();
    void failed();
    void newConnection();

public Q_SLOTS:
    void listen();

private Q_SLOTS:
    void newSocket();
    void readData();
    void removeSocket();

private:
    void runCommand(QObject *sender, QByteArray command);
    void terminateProcess(QObject *sender);
    void sendResponse(QObject *receiver, RedoParser::ResponseType code, const QByteArray &response);
    void readyOutput(QObject *sender);
    void readyError(QObject *sender);
    void finished(QObject *sender, int exitCode, QProcess::ExitStatus exitStatus);

private:
    RedoServerPrivate *p;
};

#endif // ASEMANSERVER_H
