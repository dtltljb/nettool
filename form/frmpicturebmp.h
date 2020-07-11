#ifndef FRMPICTUREBMP_H
#define FRMPICTUREBMP_H

#include <QWidget>
#include    "tcpserver.h"
#include    "bitmappic.h"
#include    "packet.h"
#include    "unpacket.h"


QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

namespace Ui {
class frmPictureBmp;
}

class frmPictureBmp : public QWidget
{
    Q_OBJECT

public:
    explicit frmPictureBmp(QWidget *parent = 0);
    ~frmPictureBmp();

private:
    Ui::frmPictureBmp *ui;
    bool isOk;
    TcpServer   *tcpServer;
    QTimer  *timer;
    bool loadFile(const QString &fileName);
    QLabel *imageLabel;
    BitmapClass     bmp ;
    Packet          *packet;
    unpacket        *unpack;
    crcCheckSum     crc;
    Protocol_Format_s   pfs;
    QVariant    var;
    char    MonitorEnableFlag = 'm';
public slots:
    bool    displayBMP(const QString fileName);

private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void append(int type, const QString &data, bool clear);

private slots:
    void sendData(const QString &data);

    void clientConnected(const QString &ip, int port);
    void clientDisconnected(const QString &ip, int port);
    void sendData(const QString &ip, int port, const QString &data);
    void receiveData(const QString &ip, int port, const QString &data);
    void ProtocolDoneProcess(const QString &ip,int port,app_socket_t *clientData);

private slots:
    void on_btnListen_clicked();
    void on_btnOpen_clicked();
    void on_btnSave_clicked();
    void on_btnClear_clicked();
    void on_btnSend_clicked();


};

#endif // FRMPICTURE_H
