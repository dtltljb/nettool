#include    <QCoreApplication>
#include    <QStandardPaths>
#include    <QDir>
#include    <QFile>
#include    <QDateTime>
#include    <QByteArray>

#include "unpacket.h"

unpacket::unpacket(QObject *parent) : QObject(parent)
{
    mAppSocket.deviceID = 0x04030201;
    mAppSocket.ReceiveDataLen   =   0x0000;
    pAppSocket  =   &mAppSocket;
    pAppSocket->rxArray  =   &AppReceive;
    pAppSocket->txArray  =   &AppSender;

}

unpacket::~unpacket()
{

}

char    unpacket::getIsReceiver()
{
    return  pAppSocket->rxArray->isReceive;
}

char    unpacket::getFrameTyep()
{
    return  pAppSocket->rxArray->FrameType;
}

void    unpacket::setClientIP_Port(QString str)
{
    pAppSocket->clientInfo = str;
    }

QByteArray unpacket::codeUnchange(QByteArray &buf)
{
    QByteArray  data;
    uint16_t    LenUnchange,len = 0;
    char magicHead = MAGIC_HEAD,magicTail=MAGIC_TAIL;
    char code_0x7D = 0x7d,code_0xFC = 0xfc;

    LenUnchange = buf.length();
    if( (buf.at(0) != magicHead)||(buf.at(buf.length()-1) != magicTail))
        return NULL;

    data.append( buf.at(len));
    for(len = 1;len < LenUnchange-1;)               /* head & tail don't unchange code */
    {
        if (code_0x7D == buf.at(len))
        {
            if( 0x01 == buf.at(len+1) )
                data.append(code_0x7D);
            else
                data.append(code_0xFC);
            len +=2;
        }
        else {
            //data.append(data.number( buf.at(len),16));
            data.append( buf.at(len));
            len += 1;
        }
    }
    data.append( buf.at(len));
    return data;
}

/*
 *  input  para: QByteArray *buf
 *  output para:Protocol_Format_s *pfs
 *  return vaue:int32_t 0 is success, app_rx_t data
*/
int32_t unpacket::socket_data_unpack_fun(Protocol_Format_s *pfs,QByteArray &buf)
{
    uint32_t    Val,crcU32;
   // QVariant    var,*pVar ;
    //Protocol_Format_s   *pfs;
    uint16_t i,base;
    app_rx_t    *pAR = &AppReceive;

    char magicHead = MAGIC_HEAD,magicTail = MAGIC_TAIL;
    if(pfs == NULL){
         qDebug()<<"Protocol_Format_s  is Null!";
        return (uint32_t)-1;
    }
   // pfs = (Protocol_Format_s*)buf.data();
    if( (buf.at(0) != magicHead)||(buf.at(buf.length()-1) != magicTail) )
    {
        qDebug()<<"receive data frame head || frame tail is error!";
        return (uint32_t)-2;
    }
    i   = 15;
    pfs->nLength    =   (uint8_t)buf.at(--i) ;            /* over 0x80 nactive has error */
    pfs->nLength   <<= 8;
    pfs->nLength    |=  (uint8_t)buf.at(--i);

    if(pfs->nLength < sizeof(App_Format_s))
    {
        qDebug()<<"receive data length is less than frame header!";
        return (uint32_t)-3;
    }
    i = buf.length();
    Val    =   i - sizeof(crcU32) - sizeof(MAGIC_TAIL)-1;
    Val = Check.Crc32(buf,Val);
    i = buf.length()-1;
    crcU32  =    (uint8_t)buf.at(--i);
    crcU32 <<= 8;
    crcU32  |=   (uint8_t)buf.at(--i);
    crcU32 <<= 8;
    crcU32  |=   (uint8_t)buf.at(--i);
    crcU32 <<= 8;
    crcU32  |=   (uint8_t)buf.at(--i);

    if(Val == crcU32)
    {
        qDebug()<<"receive data crc32 check is error!";
        return (uint32_t)-4;
    }
    i=5;
    pfs->ID     =   (uint8_t)buf.at(--i);
    pfs->ID     <<= 8;
    pfs->ID    |=   (uint8_t)buf.at(--i);
    pfs->ID     <<= 8;
    pfs->ID    |=   (uint8_t)buf.at(--i);
    pfs->ID     <<= 8;
    pfs->ID    |=   (uint8_t)buf.at(--i);
    if(pAppSocket->deviceID != pfs->ID)
    {
        qDebug()<<"receive ID is error!";
        return (uint32_t)-7;
    }
    i   = 13;
    pfs->FrameSequ    =   (uint8_t)buf.at(--i);
    pfs->FrameSequ    <<= 8;
    pfs->FrameSequ    |=  (uint8_t)buf.at(--i);
    pfs->FrameTotal    =   (uint8_t)buf.at(--i);
    pfs->FrameTotal   <<= 8;
    pfs->FrameTotal    |=  (uint8_t)buf.at(--i);
    if(pfs->FrameTotal== 0x0000 )
    {
        base = PROTOCOL_FIX_LENGTH;
        for(i=0;i < pfs->nLength/* - sizeof(crcU32)*/;i++)      /*copy app layer data*/
        {
            //pAR->rxArray.at(i) = buf.at(base + i);
            pAR->rxArray.append( buf.at(base + i) );
            if(i > FRAME_MAX_SIZE)
            {
                qDebug()<<"receive data Length is error!";
                return (uint32_t)-5;
            }
        }
        /* return pfs values */
        i = 8;
        pfs->FrameType  = buf.at(--i);
        pfs->FrameIndex =  (uint8_t)buf.at(--i);
        pfs->FrameIndex <<= 8;
        pfs->FrameIndex |= (uint8_t)buf.at(--i);
        pfs->FrameSequ  =   0x0000;
        pfs->FrameTotal =  0x0000;
        pfs->pData      =  (uint8_t*) pAR->rxArray.data();        /*update pointer*/
        /*fill app_rx_t context*/
        pAR->FrameTotal =   0x0;
        pAR->isReceive  =   'S';                                /* single frame output flag */
        pAR->FrameIndex = pfs->FrameIndex;
        pAR->FrameType  = pfs->FrameType;

        /* emit single to done process */
        QStringList list = pAppSocket->clientInfo.split(":");
        emit sendProtocolContex(list.at(0),list.at(1).toInt(),pAppSocket);
        return  0;
    }
    else{ /* receive  is multiple frame data */
        if (pfs->FrameTotal>= APP_FRAME_COUNT)
        {
            qDebug()<<"receive data frame is big multiple,Error.. !";
            return (uint32_t)-6;
        }
        else if(pfs->FrameSequ == pfs->FrameTotal)
        {
            base = PROTOCOL_FIX_LENGTH;
            for(i=0;i < pfs->nLength/* - sizeof(crcU32)*/;i++)          /*copy app layer data*/
            {
                //pAR->rxArray.at(i) = buf.at(base + i);
                pAR->rxArray.append( buf.at(base + i) );
                if(i > FRAME_MAX_SIZE)
                {
                    qDebug()<<"receive data Length is error!";
                    return (uint32_t)-5;
                }
            }
            /* return pfs values */
            i = 8;
            pfs->FrameType  = buf.at(--i);
            pfs->FrameIndex =  (uint8_t)buf.at(--i);
            pfs->FrameIndex <<= 8;
            pfs->FrameIndex |= (uint8_t)buf.at(--i);

            /*fill app_rx_t context*/
            pAR->isReceive  =   'M';        /*  multple frame output */
            pAR->FrameTotal =   pfs->FrameTotal;
            pAR->FrameIndex = pfs->FrameIndex;
            pAR->FrameType  = pfs->FrameType;
            pAR->FrameSequ  = pfs->FrameSequ;
            pAR->FrameTotal = pfs->FrameTotal;

            /* emit single to done process */
            QStringList list = pAppSocket->clientInfo.split(":");
            emit sendProtocolContex(list.at(0), list.at(1).toInt(), pAppSocket);

            return 0;
        }
        else if(pfs->FrameSequ == (uint16_t)0x0001)
        {
            base = PROTOCOL_FIX_LENGTH;
            uint16_t    ilen = pfs->nLength/* - sizeof(crcU32)*/;
            for(i=0;i < ilen;i++)      /*copy app layer data*/
            {
                //pAR->rxArray.at(i) = buf.at(base + i);
                pAR->rxArray.append( buf.at(base + i) );
                if(i > FRAME_MAX_SIZE)
                {
                    qDebug()<<"receive data Length is error!";
                    return (uint32_t)-5;
                }
            }
            /* return pfs values */
            i = 8;
            pfs->FrameType  = buf.at(--i);
            pfs->FrameIndex =  (uint8_t)buf.at(--i);
            pfs->FrameIndex <<= 8;
            pfs->FrameIndex |= (uint8_t)buf.at(--i);
            pfs->pData      =  (uint8_t*) pAR->rxArray.data();        /*update pointer*/
            /*fill app_rx_t context*/
            pAR->isReceive  =   ' ';
            pAR->FrameIndex = pfs->FrameIndex;
            pAR->FrameType  = pfs->FrameType;
            pAR->FrameSequ  = pfs->FrameSequ;
            pAR->FrameTotal = pfs->FrameTotal;
            /* fill app_socket_t context */
            //pAppSocket->deviceID    = pfs->ID;

        }
        else
        if( ((pfs->FrameSequ-1) == pAR->FrameSequ ) ) {
            base = PROTOCOL_FIX_LENGTH;
            for(i=0;i < pfs->nLength/*- sizeof(crcU32)*/;i++)      /*copy app layer data*/
            {
                //pAR->rxArray.at(i) = buf.at(base + i);
                pAR->rxArray.append( buf.at(base + i) );
                if(i > FRAME_MAX_SIZE)
                {
                    qDebug()<<"receive data Length is error!";
                    return (uint32_t)-5;
                }
            }
            /* return pfs values */
            i = 8;
            pfs->FrameType  = buf.at(--i);
            pfs->FrameIndex =  (uint8_t)buf.at(--i);
            pfs->FrameIndex <<= 8;
            pfs->FrameIndex |= (uint8_t)buf.at(--i);

            /*fill app_rx_t context*/
            pAR->isReceive  =   ' ';
            pAR->FrameIndex = pfs->FrameIndex;
            pAR->FrameType  = pfs->FrameType;
            pAR->FrameSequ  = pfs->FrameSequ;
            pAR->FrameTotal = pfs->FrameTotal;
            /* fill app_socket_t context */
            //pAppSocket->deviceID    = pfs->ID;
        }
        else{
            pAR->rxArray.clear();           /*clear array context*/
            pAR->FrameIndex = 0x00;
            pAR->FrameType  = 0x00;
            pAR->FrameSequ  = 0x00;
            pAR->FrameTotal = 0x00;
            qDebug()<<"receive data frame is error multiple !";
            return (uint32_t)-6;
        }
        return 1;
    }

}
/*
 *  input:QByteArray &buf
 *  output:QByteArray clientData->data
*/

QByteArray unpacket::DivideFrame(QByteArray &buf)
{
    QByteArray  data;
    uint32_t    len;
    char magicHead = MAGIC_HEAD,magicTail = MAGIC_TAIL;
    char isHeader = 'n',isTail = 'n';
    uint16_t u16,headIndex ,tailIndex ;

    /* find complete frame */
    for(len = 0; len < buf.length();len++)
    {
        data.append(buf.at(len));
        if( (isHeader != 'y') && (buf.at(len) == magicHead) )
        {
           headIndex = len;
           isHeader =   'y';
        }
        else if(  (isHeader == 'y')&&(buf.at(len) == magicTail)&&(isTail!='y')  )
        { /* cut into frame context */
            tailIndex = len;
            isTail =   'n';
            isHeader = 'n';
            u16 = len - headIndex;
            buf.remove(headIndex,u16+1);     /* delete cut into frame*/
            return data;
        }
        else{
            /* nothing do */
        }
    }
    /* clear context ,exit release clientData->data */
     data.clear();
     return data;
}

/*
 * input para:QByteArray    &data
 * output para: none
 *
*/

#define MATRIX_SIZE Max_x*Max_y*3

void unpacket::protocolAnalysis(QByteArray  &data)
{
    int32_t   res;
    QByteArray  idata;
    Protocol_Format_s   pfs;
    app_socket_t *clientData;
    clientData = &mAppSocket;
    //clientData->data.append(QUIHelper::hexStrToByteArray(data));    /* restore to global data paraments*/
    clientData->data.append(data);    /* restore to global data paraments*/
     while(1)
     {
         idata = DivideFrame(clientData->data);
         if(idata.isEmpty())
             return;
         idata = codeUnchange(idata);
         res = socket_data_unpack_fun(&pfs,idata);
         idata.clear();
         if(res == 0x0000)
         {
             char c = getIsReceiver();
             if(c == 'S')
             {
                 /* done process common command */
                 qDebug()<<"receive single frame "<<endl;
             }
             else if(c == 'M')
             {
                 QString str = QString("%s").arg(clientData->rxArray->rxArray.length(),5,10);
                 qDebug()<<"receive multiple frame,length= "+ str<< endl;

                 /* save bmp picture */
                 /*part.1 set file path & name */
                 QDir dir ;
                 QDateTime datetime =QDateTime::currentDateTime();
                 str = datetime.toString("yyyyMMdd");
                 dir.mkdir(str);
                 QString pdir = dir.currentPath();
                 pdir.append("/" + str+ "/");
                 str = datetime.toString("hh:mm:ss.zzz");
                 str.append(".bmp");
                 pdir.append(str);
                 bmp.setPathName(pdir);
                 /*part.2 matrix picture data area*/
                 quint8 *bits = (quint8 *)malloc( MATRIX_SIZE );                        /*-- MATRIX_SIZE = Max_x*Max_y*3  --*/
                 uint16_t i,len = (clientData->rxArray->rxArray.length() - sizeof(App_Format_s)) / 2 ;
                 uint16_t index,ADCValue,base = sizeof(App_Format_s);

                 QByteArray rgbValue;
                 for(i=0;i< len ;i++)
                 {
                    ADCValue = (uint8_t)clientData->rxArray->rxArray.at( i*2 + base );
                    ADCValue |= clientData->rxArray->rxArray.at( i*2 + 1 + base )<<8;
                    index   = pConvert.getPressuredColor(ADCValue);
                    rgbValue = colorCard.getColorCard(index);
                    memcpy((bits+i*3), rgbValue.data(), 3);
                 }
                /* part.3  write data to picture */
                 bmp.saveBitmapFile(Max_x,Max_y,(unsigned char*)bits);
                 clientData->data.clear();
                 clientData->rxArray->rxArray.clear();
                 /*part.4 emit send BMP file  */
                 emit   SendBMP(pdir);
             }
             else{
                 qDebug()<<"receive frame sequ...,continue.."<<endl;
             }
         }
         else{
             /* nothing */
         }
     }

}
