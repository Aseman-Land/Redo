#ifndef REDOCLIENT_H
#define REDOCLIENT_H

#include <QObject>
#include <QTcpSocket>

#include "redoparser.h"
#include "redoclient_global.h"

class RedoClientPrivate;
class REDOCLIENT_EXPORT RedoClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)

public:
    RedoClient(QObject *parent = nullptr);
    virtual ~RedoClient();

    void setHost(const QString &host);
    QString host();

    void setPort(quint16 port);
    quint16 port();

Q_SIGNALS:
    void hostChanged();
    void portChanged();
    void connected();
    void failed();
    void readyOutput(const QByteArray &output);
    void readyError(const QByteArray &output);
    void finished();

public Q_SLOTS:
    void connect();
    void sendRequest(RedoParser::RequestType code);
    void sendRequest(RedoParser::RequestType code, const QByteArray &command);

private Q_SLOTS:
    void readData();

private:
    RedoClientPrivate *p;
};

#endif // ASEMANCLIENT_H
