#include    <QCoreApplication>
#include    <QStandardPaths>

#include "frmpicturebmp.h"
#include "ui_frmpicturebmp.h"
#include "quiwidget.h"
#include "crc32.h"



frmPictureBmp::frmPictureBmp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmPictureBmp)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
//    imageLabel = new QLabel;
//    imageLabel->setBackgroundRole(QPalette::Base);
//    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    imageLabel->setScaledContents(true);

}

frmPictureBmp::~frmPictureBmp()
{
    delete ui;
}

void frmPictureBmp::initForm()
{
    isOk = false;

    tcpServer =new TcpServer(this);
    connect(tcpServer,SIGNAL(clientConnected(QString,int)),this,SLOT(clientConnected(QString,int)));
    connect(tcpServer,SIGNAL(clientDisconnected(QString,int)),this,SLOT(clientDisconnected(QString,int)));
    connect(tcpServer,SIGNAL(sendData(QString,int,QString)),this,SLOT(sendData(QString,int,QString)));
    connect(tcpServer,SIGNAL(receiveData(QString,int,QString)),this,SLOT(receiveData(QString,int,QString)));


    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(on_btnSend_clicked()));
    ui->cboxInterval->addItems(App::Intervals);
    ui->cboxData->addItems(App::Datas);
    QUIHelper::setLabStyle(ui->labCount, 3);

    packet  =   new Packet();
    unpack  =   new unpacket();

}

void frmPictureBmp::initConfig()
{
       ui->ckHexSend->setChecked(App::HexSendTcpServer);
       connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

       ui->ckHexReceive->setChecked(App::HexReceiveTcpServer);
       connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

       ui->ckAscii->setChecked(App::AsciiTcpServer);
       connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

//       ui->ckDebug->setChecked(App::DebugTcpServer);
//       connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

       ui->ckAutoSend->setChecked(App::AutoSendTcpServer);
       connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

       ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(App::IntervalTcpServer)));
       connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

       ui->txtListenPort->setText(QString::number(App::TcpListenPort));
       connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

       timer->setInterval(App::IntervalTcpServer);
       App::AutoSendTcpServer ? timer->start() : timer->stop();
}


void frmPictureBmp::saveConfig()
{
    App::HexSendTcpServer = ui->ckHexSend->isChecked();
    App::HexReceiveTcpServer = ui->ckHexReceive->isChecked();
    App::AsciiTcpServer = ui->ckAscii->isChecked();
//    App::DebugTcpServer = ui->ckDebug->isChecked();
    App::AutoSendTcpServer = ui->ckAutoSend->isChecked();
    App::IntervalTcpServer = ui->cboxInterval->currentText().toInt();
    App::TcpListenPort = ui->txtListenPort->text().trimmed().toInt();
    App::writeConfig();

    timer->setInterval(App::IntervalTcpServer);
    App::AutoSendTcpServer ? timer->start() : timer->stop();
}

void frmPictureBmp::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;


    currentCount++;
}

void frmPictureBmp::sendData(const QString &data)
{
    if (ui->ckAll->isChecked()) {
        tcpServer->writeData(data);
    } else {
        int row = ui->listWidget->currentRow();
        if (row >= 0) {
            QString str = ui->listWidget->item(row)->text();
            QStringList list = str.split(":");
            tcpServer->writeData(list.at(0), list.at(1).toInt(), data);
        }
    }
}

void frmPictureBmp::clientConnected(const QString &ip, int port)
{
    QString str = QString("%1:%2").arg(ip).arg(port);
    ui->listWidget->addItem(str);
    ui->labCount->setText(QString("共 %1 个连接").arg(ui->listWidget->count()));
}

void frmPictureBmp::clientDisconnected(const QString &ip, int port)
{
    int row = -1;
    QString str = QString("%1:%2").arg(ip).arg(port);
    for (int i = 0; i < ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->text() == str) {
            row = i;
            break;
        }
    }

    ui->listWidget->takeItem(row);
    ui->labCount->setText(QString("共 %1 个连接").arg(ui->listWidget->count()));
}

void frmPictureBmp::sendData(const QString &ip, int port, const QString &data)
{
    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    bool error = (data.contains("下线") || data.contains("离线"));
   // append(error ? 1 : 0, str);
}

/*
 * socket receiver
*/
void frmPictureBmp::receiveData(const QString &ip, int port, const QString &data)
{

    QByteArray  idata;
    QString str ;
    unpack->setClientIP_Port(QString("%1:%2").arg(ip).arg(port));
    str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    qDebug()<<str;
    idata.append(QUIHelper::hexStrToByteArray(data));
    unpack->protocolAnalysis(idata);
}

bool frmPictureBmp::displayBMP(const QString fileName)
{
    /* part.1 load image */
    if(fileName.isNull())
    {
        qDebug()<<"displayBMP file is null...error!";
        return false;
    }
    QImage image;
    image.load(fileName,0);
    if(image.isNull())
    {
        qDebug()<<"BMP file is null...error!";
        return false;
    }
    /*part.2 display bmp file*/
    ui->labelPic->clear();
    ui->labelPic->setScaledContents(true);
    ui->labelPic->setPixmap(QPixmap::fromImage(image));
    ui->labelPic->adjustSize();
    setWindowFilePath(fileName);
    return true;
}

bool frmPictureBmp::loadFile(const QString &fileName)
{
   // bmp.ReadBitmapFile(fileName);
    QImage image;
    image.load(fileName,0);
    if (image.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
//        imageLabel->setPixmap(QPixmap());
//        imageLabel->adjustSize();
        return false;
    }
//! [2] //! [3]
//!
    //image.scaled(ui->labelPic->size(),Qt::KeepAspectRatio);
    ui->labelPic->clear();
    ui->labelPic->setScaledContents(true);
    ui->labelPic->setPixmap(QPixmap::fromImage(image));
    ui->labelPic->adjustSize();
//! [3] //! [4]
//    scaleFactor = 1.0;

//    printAct->setEnabled(true);
//    fitToWindowAct->setEnabled(true);
//    updateActions();

//    if (!fitToWindowAct->isChecked())
//        imageLabel->adjustSize();

    setWindowFilePath(fileName);
    return true;
}



void frmPictureBmp::on_btnListen_clicked()
{
    if (ui->btnListen->text() == "监听") {
        isOk = tcpServer->start();
        if (isOk) {
         //   append(0, "监听成功");
            ui->btnListen->setText("关闭");
        }
    } else {
        isOk = false;
        tcpServer->stop();
        ui->btnListen->setText("监听");
    }
}

void frmPictureBmp::on_btnOpen_clicked()
{

    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}

}


void frmPictureBmp::on_btnSave_clicked()
{
    bmp.saveBitMap();
    on_btnClear_clicked();
}

void frmPictureBmp::on_btnClear_clicked()
{
    append(0, "", true);
}

void frmPictureBmp::on_btnSend_clicked()
{
    QString str,s;
    QByteArray  buffer;

    if (!isOk) {
        return;
    }
    if(MonitorEnableFlag == 'm')
    {
        ui->btnSend->setText("stop");
        connect(unpack,SIGNAL(sendProtocolContex(QString,int,app_socket_t*)),this,SLOT(ProtocolDoneProcess(QString,int,app_socket_t*)));
        connect(unpack,SIGNAL(SendBMP(QString)),this,SLOT(displayBMP(QString)));
        pfs.ID          =   0x01020304;
        pfs.FrameType   =   READMATRIXPRESSURE;
        packet->setProtocolFrameIndex(packet->get_Protocol_frame_index()+1);
        pfs.FrameIndex  =packet->get_Protocol_frame_index();
    //    pfs.FrameIndex  =0x0102;
        pfs.FrameType   =   P_DATA_RQ;
        pfs.FrameSequ   = 0x0;
        pfs.FrameTotal  =   0;
        var.setValue(pfs);
        buffer = "050071000000";
        str = packet->socket_data_pack_func(var,buffer);
        buffer = QUIHelper::hexStrToByteArray(str);
        buffer = packet->codeChange(buffer);
        s = QUIHelper::byteArrayToHexStr(buffer);
        ui->cboxData->setCurrentText(s);
        sendData(s);
        MonitorEnableFlag = 'n';
    }
   else{
        disconnect(unpack,SIGNAL(sendProtocolContex(QString,int,app_socket_t*)),this,SLOT(ProtocolDoneProcess(QString,int,app_socket_t*)));
        disconnect(unpack,SIGNAL(SendBMP(QString)),this,SLOT(displayBMP(QString)));
        MonitorEnableFlag = 'm';
        ui->btnSend->setText("start");
    }
}

void frmPictureBmp::ProtocolDoneProcess(const QString &ip, int port, app_socket_t *clientData)
{
    qDebug()<< ip << ":"<<port << "cmd="<<clientData->rxArray->isReceive;

    QString str,s;
    QByteArray  buffer;

    if (!isOk) {
        return;
    }
    pfs.ID          =   0x01020304;
    pfs.FrameType   =   READMATRIXPRESSURE;
    packet->setProtocolFrameIndex(packet->get_Protocol_frame_index()+1);
    pfs.FrameIndex  =packet->get_Protocol_frame_index();
//    pfs.FrameIndex  =0x0102;
    pfs.FrameType   =   P_DATA_RQ;
    pfs.FrameSequ   = 0x0;
    pfs.FrameTotal  =   0;
    var.setValue(pfs);
    buffer = "050071000000";
    str = packet->socket_data_pack_func(var,buffer);
    buffer = QUIHelper::hexStrToByteArray(str);
    buffer = packet->codeChange(buffer);
    s = QUIHelper::byteArrayToHexStr(buffer);
    ui->cboxData->setCurrentText(s);
    sendData(s);

}
