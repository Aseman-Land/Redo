#include <QCoreApplication>

#include "redoserver.h"
#include "config.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Config serverConfig;
    serverConfig.loadConfig();

    RedoServer server;
    server.setHost(serverConfig.getHost());
    server.setPort(serverConfig.getPort());
    server.setWhitelist(serverConfig.getWhiteList());
    server.listen();

    return a.exec();
}
