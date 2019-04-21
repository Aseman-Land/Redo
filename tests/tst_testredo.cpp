#include <QtTest>
#include <QSignalSpy>

// add necessary includes here
#include "redoserver.h"
#include "redoclient.h"
#include "redoparser.h"
#include <QCoreApplication>

const QString host = "127.0.0.1";
const quint16 port = 3000;
static qint32 numberOfConnections = 500;
static qint32 socketsConnected = 0;
static qint32 readyOutput = 0;

class TestRedo : public QObject
{
    Q_OBJECT

public:
    TestRedo();
    ~TestRedo();

private slots:
    void test_singleConnection();
    void test_multiConnection();
};

TestRedo::TestRedo()
{

}

TestRedo::~TestRedo()
{

}

void TestRedo::test_singleConnection()
{
    QEventLoop loop;
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timer.start(100);

    RedoServer server;
    QSignalSpy serverListenSpy(&server, SIGNAL(started()));
    QSignalSpy connectionSpy(&server, SIGNAL(newConnection()));
    server.setHost(host);
    server.setPort(port);
    server.listen();
    
    RedoClient client;
    QSignalSpy socketConnectSpy(&client, SIGNAL(connected()));
    client.setHost(host);
    client.setPort(port);

    QTimer::singleShot(10, &client, &RedoClient::connect);

    loop.exec();

    QCOMPARE(serverListenSpy.count(), 1);
    QCOMPARE(connectionSpy.count(), 1);
    QCOMPARE(socketConnectSpy.count(), 1);
}

void TestRedo::test_multiConnection()
{
    QEventLoop loop;
    QTimer timer;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(2000);

    RedoServer server;
    QSignalSpy serverListenSpy(&server, SIGNAL(started()));
    QSignalSpy connectionSpy(&server, SIGNAL(newConnection()));
    server.setHost(host);
    server.setPort(port);
    server.listen();

    QTimer::singleShot(20, [&](){

        class ClientTask : public QRunnable
        {
            void run() override
            {
                for(int i = 0; i < numberOfConnections; i++) {
                    RedoClient *client = new RedoClient();
                    connect(client, &RedoClient::connected, [client](){
                        socketsConnected++;

                        QByteArray request;
                        QDataStream requestStream(&request, QIODevice::WriteOnly);
                        requestStream << QStringList({"ls"});

                        client->sendRequest(RedoParser::RunCommand, request);
                    });
                    connect(client, &RedoClient::readyOutput, [](){
                        readyOutput++;
                    });
                    client->setHost(host);
                    client->setPort(port);
                    client->connect();
                }
            }
        };

        ClientTask *socket = new ClientTask();
        QThreadPool::globalInstance()->start(socket);

    });
    loop.exec();

    QCOMPARE(serverListenSpy.count(), 1);
    QCOMPARE(connectionSpy.count(), numberOfConnections);
    QCOMPARE(socketsConnected, numberOfConnections);
    // server not reading data in test fix this
//    QCOMPARE(readyOutput, numberOfConnections);
}

QTEST_MAIN(TestRedo)

#include "tst_testredo.moc"
