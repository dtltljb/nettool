#include    "packet.h"
#include    "qdebug.h"
#include    "crc32.h"
#include "quiwidget.h"

Packet::Packet()
{
//    Protocol_Format_s   fixHead;
//    QVariant    dataVar;
//    dataVar.setValue(fixHead);
//    qRegisterMetaType<QVariant>("Protocol_Format_s");
}

Packet::~Packet()
{

}

/********************************************************************************************************
**@brief		void ChangeDefine(uint8_t *r_data, int *r_len)
**input		:	 uint8_t *r_data, int *r_len
**output	:	 uint8_t *r_data, int *r_len
**
**
********************************************************************************************************/

QByteArray Packet::codeChange (const QByteArray &rdata)
{
    uint16_t ilen=0;
    QByteArray  ibuf;
    char    samble  =   0x7d;
    char    magic   =   0xfc;
    uint16_t len = rdata.length();
    quint16 i=0;
    if( (rdata.at(i) != magic) || (rdata.at(rdata.length()-1) != magic) )
        return NULL;
    ibuf[ilen++] =rdata.at(i);
    for(i=1;i < len-1;i++)
    {
     if(rdata.at(i) == samble){
        ibuf[ilen++] = 0x7d;
        ibuf[ilen++] = 0x01;
     }
     else if(rdata.at(i) == magic){
         ibuf[ilen++] = 0x7d;
         ibuf[ilen++] = 0x02;
     }
     else{
         ibuf[ilen++] = rdata.at(i);
     }

    }
    ibuf[ilen++] = rdata.at(i);     /* tail samble */
    return ibuf;
}

/********************************************************************************************************
**	@brief	 uint32_t socket_data_pack_func(void *args, uint8_t **r_data, uint16_t *r_len)
**	input:	 void *args (NSFPFixHead);
**	output packet result:	 uint8_t *r_data, int *r_len
**	=> data length fill 8 bytes => crc32 => des encypt
********************************************************************************************************/

QByteArray Packet::socket_data_pack_func(QVariant var, const QByteArray &rdata)
{

    quint32  		Crc32;
    QByteArray		buf,obuffer;
    quint8  u8 = 0;

//    QString str;
//    Protocol_Format_s fix_head;

    if(!var.canConvert<Protocol_Format_s>())
        return NULL;
    else
        fix_head = var.value<Protocol_Format_s>();
    fix_head.MagicHead 	= 	MAGIC_HEAD;
    if( fix_head.FrameIndex == u8){
        m_Frame_index++;
        fix_head.FrameIndex  	=	m_Frame_index;
    }

    fix_head.ProtocolVer 	= 	PROTOCOL_VERSION;
    fix_head.nLength    = rdata.length()/2+ sizeof(Crc32);
    fix_head.pData  =  (uint8_t*)rdata.data();  /*response pointer*/
    fix_head.MagicTail 	= 	MAGIC_TAIL;

    obuffer.append(QString("%1").arg(fix_head.MagicHead,2,16,QLatin1Char('0')));

    obuffer.append(QString("%1").arg(fix_head.ID,8,16,QLatin1Char('0')) );

    obuffer.append(QString("%1").arg(fix_head.FrameIndex,4,16,QLatin1Char('0')));

    obuffer.append(QString("%1").arg(fix_head.FrameType,2,16,QLatin1Char('0')));
    obuffer.append(QString("%1").arg(fix_head.ProtocolVer,2,16,QLatin1Char('0')));

    obuffer.append(QString("%1").arg(fix_head.FrameTotal,4,16,QLatin1Char('0')));
    obuffer.append(QString("%1").arg(fix_head.FrameSequ,4,16,QLatin1Char('0')));
    obuffer.append(QString("%1").arg(fix_head.nLength,4,16,QLatin1Char('0')));

    obuffer.append(rdata);
    Crc32 = Check.Crc32(obuffer.data(),obuffer.length());
    obuffer.append(QString("%1").arg(Crc32,8,16,QLatin1Char('0')) );
    obuffer.append(QString("%1").arg(fix_head.MagicTail,2,16,QLatin1Char('0')));

    return	obuffer;
}



quint16 Packet::get_Protocol_frame_index()
{
    return m_Frame_index;
}

void Packet::setProtocolFrameIndex(quint16 val)
{
        m_Frame_index = val;
}
