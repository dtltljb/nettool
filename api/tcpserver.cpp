#include "tcpserver.h"
#include "quiwidget.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{

}

#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
void TcpServer::incomingConnection(qintptr handle) /* QT5.5 version input parament */
#else
void TcpServer::incomingConnection(int handle)
#endif
{
    TcpClient *client = new TcpClient(this);
    client->setSocketDescriptor(handle);
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(sendData(QString, int, QString)), this, SIGNAL(sendData(QString, int, QString)));
    connect(client, SIGNAL(receiveData(QString, int, QString)), this, SIGNAL(receiveData(QString, int, QString)));

    QString ip = client->peerAddress().toString();
    int port = client->peerPort();
    client->setIP(ip);
    client->setPort(port);
    emit clientConnected(ip, port);
    emit sendData(ip, port, "客户端上线");

    //连接后加入链表
    clients.append(client);
}

void TcpServer::disconnected()
{
    TcpClient *client = (TcpClient *)sender();
    QString ip = client->getIP();
    int port = client->getPort();
    emit clientDisconnected(ip, port);
    emit sendData(ip, port, "客户端下线");

    //断开连接后从链表中移除
    clients.removeOne(client);
}

bool TcpServer::start()
{
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    //bool ok = listen(QHostAddress::AnyIPv4, App::TcpListenPort);
    bool ok = listen(QHostAddress::AnyIPv4, App::TcpListenPort);
#else
    bool ok = listen(QHostAddress::Any, App::TcpListenPort);
#endif

    return ok;
}

void TcpServer::stop()
{
    foreach (TcpClient *client, clients) {
        client->disconnectFromHost();
    }

    this->close();
}

void TcpServer::writeData(const QString &ip, int port, const QString &data)
{
    foreach (TcpClient *client, clients) {
        if (client->peerAddress().toString() == ip && client->peerPort() == port) {
            client->sendData(data);
            break;
        }
    }
}

void TcpServer::writeData(const QString &data)
{
    foreach (TcpClient *client, clients) {
        client->sendData(data);
    }
}
