#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QtNetwork>
#include "crc32.h"
#include "packet.h"

class TcpClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = 0);

private:
    QString ip;
    int port;

    Packet packet;

public:
    void setIP(const QString &ip);
    QString getIP()const;

    void setPort(int port);
    int getPort()const;

private slots:
    void readData();

signals:
    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

public slots:
    void sendData(const QString &data);

};

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0);

private:
    QList<TcpClient *> clients;

protected:
    void incomingConnection(qintptr handle);

private slots:
    void disconnected();

signals:
    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);

    void  clientConnected(const QString &ip, int port);
    void  clientDisconnected(const QString &ip, int port);

public slots:
    //启动服务
    bool start();
    //停止服务
    void stop();

    //指定连接发送数据
    void writeData(const QString &ip, int port, const QString &data);
    //对所有连接发送数据
    void writeData(const QString &data);

};

#endif // TCPSERVER_H
