#ifndef CONFIG_H
#define CONFIG_H

#include <QList>
#include <QHostAddress>

class Config
{
public:
    Config();

    QString getHost() const;
    void setHost(const QString &value);

    quint16 getPort() const;
    void setPort(const quint16 &value);

    QStringList getWhiteList() const;
    void setWhiteList(const QStringList &value);

    bool loadConfig();
    bool saveConfig();

private:
    void read(const QJsonObject &json);
    void write(QJsonObject &json);

private:
    QString host;
    quint16 port;
    QStringList whiteList;
};

#endif // CONFIG_H
