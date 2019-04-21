#include <QCoreApplication>
#include <QObject>
#include <stdio.h>
#include <QDebug>
#include <QCommandLineParser>
#include <QBuffer>
#include <QDataStream>
#include <QThread>

#include "redoclient.h"
#include "redoparser.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    RedoClient client;

    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    QCommandLineOption hostOption("host", QCoreApplication::translate("main", "Set connection host"), "Ip Address");
    parser.addOption(hostOption);
    QCommandLineOption portOption("port", QCoreApplication::translate("main", "Set connection port"), "port");
    parser.addOption(portOption);

    parser.parse(QCoreApplication::arguments());
    if(parser.isSet(hostOption)) {
        client.setHost(parser.value(hostOption));
    }

    if(parser.isSet(portOption)) {
        client.setPort(parser.value(portOption).toUShort());
    }

    const QStringList positionalArgs = parser.positionalArguments();

    client.connect();

    QByteArray request;
    QDataStream requestStream(&request, QIODevice::WriteOnly);
    requestStream << positionalArgs;

    client.sendRequest(RedoParser::RunCommand, request);

    QObject::connect(&client, &RedoClient::readyOutput, [](const QByteArray &output){
        fprintf(stdout, "%s", output.toStdString().c_str());
    });
    QObject::connect(&client, &RedoClient::readyError, [](const QByteArray &output){
        fprintf(stderr, "%s", output.toStdString().c_str());
    });


    return a.exec();
}
