#ifndef UNPACKET_H
#define UNPACKET_H

#include    <QVariant>
#include    <QByteArray>
#include    <QMetaType>
#include    <QObject>

#include    "protocolformat.h"
#include    "bitmappic.h"
#include    "pseudocolor.h"
#include    "pressuredconvert.h"
#include    "crc32.h"

#define unpacketBufSize     FRAME_MAX_SIZE*APP_FRAME_COUNT
#define frameMinLength      sizeof(Protocol_Format_s)

#define errorCodeUnpackLengthMin        (uint32_t)0x01
#define	errorCodeUnpackVersion			(uint32_t)0x02
#define	errorCodeDataLengthMin			(uint32_t)0x03
#define	errorCodeMagicTail              (uint32_t)0x04
#define	errorCodeCrcCheck               (uint32_t)0x05
#define	errorCodeFrameLengthMin			(uint32_t)0x06
#define	errorCodeMagicHeader			(uint32_t)0x07

Q_DECLARE_METATYPE(app_rx_t)
Q_DECLARE_METATYPE(app_tx_t)
Q_DECLARE_METATYPE(app_socket_t)

class unpacket : public QObject
{
    Q_OBJECT
public:
    explicit unpacket(QObject *parent = 0);
    ~unpacket();

    char    getIsReceiver();
    char    getFrameTyep();
    void    setClientIP_Port(QString str);
    QByteArray codeUnchange(QByteArray &buf);
    QByteArray DivideFrame(QByteArray &buf);
    int32_t socket_data_unpack_fun(Protocol_Format_s *pfs, QByteArray &buf);
    void        protocolAnalysis(QByteArray  &data);
private:
    app_tx_t        AppSender   =   {0};
    app_rx_t        AppReceive  =   {0};    /* gloable parament */
    app_socket_t    *pAppSocket,mAppSocket;
    crcCheckSum     Check;
    BitmapClass     bmp;
    pseudocolor     colorCard;
    pressuredConvert    pConvert;

signals:
     void   sendProtocolContex(const QString &ip,int port,app_socket_t *clientData);
     bool   SendBMP(const QString fileName);

public slots:


};

#endif // UNPACKET_H
