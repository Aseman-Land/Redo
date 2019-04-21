#ifndef REDOPARSER_H
#define REDOPARSER_H

#include <QtGlobal>
#include <QString>

#include "redoclient_global.h"

class REDOCLIENT_EXPORT RedoParser
{
public:
    enum RequestType {
        RunCommand,
        InputChannel,
        Termination
    };

    enum ResponseType {
        Output,
        Error,
        Finished
    };

    RedoParser(const QByteArray &req);
    RedoParser(qint32 code, const QByteArray &command);

    QByteArray writeRequest();

    qint32 getCode() const;
    QByteArray getCommand() const;

private:
    qint32 _code;
    QByteArray _command;
};

#endif // PROCESSREQUEST_H
