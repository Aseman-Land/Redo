#include "redoparser.h"

#include <QBuffer>
#include <QDataStream>

RedoParser::RedoParser(const QByteArray &req)
{
    QByteArray data = req;
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);

    QDataStream dataStream(&buffer);
    dataStream >> _code >> _command;
}

RedoParser::RedoParser(qint32 code, const QByteArray &command)
{
    _code = code;
    _command = command;
}

QByteArray RedoParser::writeRequest()
{
    QByteArray request;
    QDataStream requestStream(&request, QIODevice::WriteOnly);
    requestStream << _code;
    requestStream << _command;

    return request;
}

qint32 RedoParser::getCode() const
{
    return _code;
}

QByteArray RedoParser::getCommand() const
{
    return _command;
}
