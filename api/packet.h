#ifndef PACKET_H
#define PACKET_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include    <QString>
#include    <QByteArray>
#include    <QVariant>
#include 	"protocolformat.h"
#include 	"crc32.h"


//#define	Enable_Display_Packet_Data				/*define equ enbale display*/
#define ChangeCodeSize      SEND_DATA_MAX_SIZE*2   //send data + frame head << ChangeCodeSize
#define READMATRIXPRESSURE  0x05;                   //

Q_DECLARE_METATYPE(Protocol_Format_s)          /* struct type */

class Packet{
public:
     Packet();
     ~Packet();

    QByteArray socket_data_pack_func(QVariant var, const QByteArray &rdata);
    QByteArray codeChange(const QByteArray &rdata);
    QByteArray app_data_pack_func(QVariant var, const QByteArray &rdata);
    quint16 get_Protocol_frame_index();
    void setProtocolFrameIndex(quint16 val);
private:
    crcCheckSum     Check;
    quint16             m_Frame_index;
    Protocol_Format_s   fix_head;
    QVariant    dataVar;
};



#endif // PACKET_H

