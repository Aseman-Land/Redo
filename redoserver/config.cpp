#include "config.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

Config::Config()
{
    host = QStringLiteral("127.0.0.1");
    port = 80;
}

QString Config::getHost() const
{
    return host;
}

void Config::setHost(const QString &value)
{
    host = value;
}

quint16 Config::getPort() const
{
    return port;
}

void Config::setPort(const quint16 &value)
{
    port = value;
}

QStringList Config::getWhiteList() const
{
    return whiteList;
}

void Config::setWhiteList(const QStringList &value)
{
    whiteList = value;
}

bool Config::loadConfig()
{
    QFile loadFile(QStringLiteral("config.json"));

    if(!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open config file");
        return false;
    }

    QByteArray configData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(configData));

    read(loadDoc.object());

    return true;
}

bool Config::saveConfig()
{
    QFile saveFile(QStringLiteral("config.json"));

    if(!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open config file");
        return false;
    }

    QJsonObject configObject;
    write(configObject);
    QJsonDocument saveDoc(configObject);
    saveFile.write(saveDoc.toJson());

    return true;
}

void Config::read(const QJsonObject &json)
{
    host = json["host"].toString();
    port = static_cast<quint16>(json["port"].toInt());

    QJsonArray whiteListArray = json["whitelist"].toArray();
    for(int listIndex = 0; listIndex < whiteListArray.size(); listIndex++) {
        whiteList.append(whiteListArray[listIndex].toString());
    }
}

void Config::write(QJsonObject &json)
{
    json["host"] = host;
    json["port"] = port;

    if(whiteList.isEmpty()) {
        whiteList.append(QStringLiteral("127.0.0.1"));
    }
    QJsonArray whiteListArray;
    foreach(const QString whiteAddress, whiteList) {
        whiteListArray.append(whiteAddress);
    }
    json["whitelist"] = whiteListArray;
}
