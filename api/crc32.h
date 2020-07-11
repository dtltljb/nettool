#ifndef CRC32_H
#define CRC32_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QDebug>

typedef unsigned char u8;
typedef unsigned char byte;
typedef unsigned short u16;
typedef unsigned int u32;

class crcCheckSum
{
public:
    crcCheckSum();
    quint16 crc16ForModbus(const QByteArray &data);

    quint8 Crc8(const QByteArray &data);
    quint16 Crc16(const QByteArray &data);
    u32 Crc32(const QByteArray &pBuff,uint32_t len);
};


#endif // CRC32_H

