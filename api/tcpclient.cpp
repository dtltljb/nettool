
#include "tcpserver.h"
#include "quiwidget.h"


TcpClient::TcpClient(QObject *parent) :  QTcpSocket(parent)
{
    ip = "127.0.0.1";
    port = 6000;

    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(readyRead()), this, SLOT(readData()));
}

void TcpClient::setIP(const QString &ip)
{
    this->ip = ip;
}

QString TcpClient::getIP() const
{
    return this->ip;
}

void TcpClient::setPort(int port)
{
    this->port = port;
}

int TcpClient::getPort() const
{
    return this->port;
}

void TcpClient::readData()
{
    QByteArray data = this->readAll();
    if (data.length() <= 0) {
        return;
    }

    QString buffer;
    if (App::HexReceiveTcpServer) {
        buffer = QUIHelper::byteArrayToHexStr(data);
    } else if (App::AsciiTcpServer) {
        buffer = QUIHelper::byteArrayToAsciiStr(data);
    } else {
        buffer = QString(data);
    }

    emit receiveData(ip, port, buffer);

    //自动回复数据,可以回复的数据是以;隔开,每行可以带多个;所以这里不需要继续判断
    if (App::DebugTcpServer) {
        int count = App::Keys.count();
        for (int i = 0; i < count; i++) {
            if (App::Keys.at(i) == buffer) {
                sendData(App::Values.at(i));
                break;
            }
        }
    }
}

void TcpClient::sendData(const QString &data)
{
    QByteArray buffer;


    if (App::HexSendTcpServer) {
        buffer = QUIHelper::hexStrToByteArray(data);

    } else if (App::AsciiTcpServer) {
        buffer = QUIHelper::asciiStrToByteArray(data);
    } else {
        buffer = data.toLatin1();
    }
    this->write(buffer);
//    emit sendData(ip, port, s);
   emit sendData(ip, port, data);
}
