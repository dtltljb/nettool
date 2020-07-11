#if defined(__cplusplus)
 extern "C" {
#endif

#ifndef __PROTOCOL_FORMAT_H
#define __PROTOCOL_FORMAT_H

#include    <stdint.h>
#include    <stdbool.h>

#include    <QString>
#include    <QByteArray>
#include    <QVariant>
#include    <QMetaType>

/* frame work type	define */
#define		A_ASSOCIATE_RQ		0x01
#define		A_ASSOCIATE_AC		0x02
#define		A_ASSOCIATE_RJ		0x03
#define		A_RELEASE_RQ		0x05
#define		A_RELEASE_RP		0x06
#define		ABORT_Service		0x07
#define		P_DATA_RQ			0x08
#define		P_DATA_RSP			0x09


/* Return values */
typedef enum{
    CO_ERROR_NO                 = 0,
    CO_ERROR_ILLEGAL_ARGUMENT   = -1,
    CO_ERROR_OUT_OF_MEMORY      = -2,
    CO_ERROR_TIMEOUT            = -3,
    CO_ERROR_ILLEGAL_BAUDRATE   = -4,
    CO_ERROR_RX_OVERFLOW        = -5,
    CO_ERROR_RX_PDO_OVERFLOW    = -6,
    CO_ERROR_RX_MSG_LENGTH      = -7,
    CO_ERROR_RX_PDO_LENGTH      = -8,
    CO_ERROR_TX_OVERFLOW        = -9,
    CO_ERROR_TX_PDO_WINDOW      = -10,
    CO_ERROR_TX_UNCONFIGURED    = -11,
    CO_ERROR_PARAMETERS         = -12,
    CO_ERROR_DATA_CORRUPT       = -13,
    CO_ERROR_CRC                = -14
}CO_ReturnError_t;

/*	frame format define*/
#define		MAGIC_HEAD					(uint8_t)0xFC
#define		MAGIC_TAIL					(uint8_t)0xFC
#define		PROTOCOL_VERSION			(uint8_t)0x01

#pragma pack(push,1)
typedef   struct   //__Pack	__packed
{
                 uint8_t  	MagicHead;
                 uint32_t  	ID;
                 uint16_t  	FrameIndex;
                 uint8_t		FrameType;
                 uint8_t		ProtocolVer;
                 uint16_t		FrameTotal;			/* 	Mul frame total */
                 uint16_t		FrameSequ;			/*	Mul frame sequce */
                 uint16_t		nLength;			/* 	receive data length ,total 15 bytes*/
                 uint8_t		*pData;					/*	Data area pointer	*/
                 //char           *pData;
                 uint32_t		crc;
                 uint8_t		MagicTail;				/*	0xFC	*/
}__attribute__((packed))Protocol_Format_s;


//Q_DECLARE_METATYPE(Protocol_Format_s)          /* struct type */


typedef struct{
     uint8_t    cmd;
     uint16_t   index;
     uint8_t    subIndex;
     uint16_t   length;
 }App_Format_s;
#pragma pack(pop)

#define		PROTOCOL_pData_LEN		sizeof(uint8_t*)
#define		PROTOCOL_Data_CRC		sizeof(uint32_t)
#define		PROTOCOL_Magic_Tail		sizeof(uint8_t)
#define		PROTOCOL_FIX_LENGTH		sizeof(Protocol_Format_s) - PROTOCOL_pData_LEN \
                                    - PROTOCOL_Data_CRC - PROTOCOL_Magic_Tail

/* MARCO DEFINE */
#define	APP_FRAME_COUNT		6											/* 应用层分包数,最多5包 */
#define	FRAME_MAX_SIZE		1050									/* socket 帧 最大数据长度*/

#define	RECEIVE_DATA_MAX_SIZE	FRAME_MAX_SIZE
#define	SEND_DATA_MAX_SIZE		FRAME_MAX_SIZE

#define	APP_DATA_MAX_SIZE			FRAME_MAX_SIZE*APP_FRAME_COUNT		/* 应用层数据最大长度 */

#ifndef FRAME_BUFFER_SIZE
    #define FRAME_BUFFER_SIZE    (FRAME_MAX_SIZE - sizeof(Protocol_Format_s))
#endif

#pragma pack(2)
/* Received message object */
typedef struct{
  char              isReceive;
  uint16_t  		FrameIndex;					/*	receive frame index */
  char              FrameType;					/* 	receive frame type */
  uint16_t			FrameTotal;					/* 	Mul frame total */
  uint16_t			FrameSequ;                  /*	Mul frame sequce */
  uint32_t			nLength;					/* 	LLC receive data length */
  QByteArray        rxArray;                     /*  receive buffer */
}app_rx_t;

/* Transmit message object. */
typedef struct{
  uint16_t  		FrameIndex;							/*	receive frame index */
  char              FrameType;							/* 	receive frame type */
  uint16_t			FrameTotal;							/* 	Mul frame total */
  uint16_t			FrameSequ;							/*	Mul frame sequce */
  uint16_t			nLength;							/* 	LLC receive data length */
  QByteArray        txArray;                            /* transmit buffer */
  char              bufferFull;
  char              syncFlag;
}app_tx_t;												/* ALIGN_STRUCT_DWORD; */


/* socket module object. */
typedef struct{
    QString             clientInfo;                  /*  client information */
    uint32_t			deviceID;					/* 	application layer device id */
    app_rx_t            *rxArray;
    app_tx_t            *txArray;
    /* divide frame */
    uint32_t            ReceiveDataLen;
    QByteArray          data;
    char                isHeader;
    uint16_t            headIndex;
    char                isTail;
    uint16_t            tailIndex;
}app_socket_t;
 #pragma pack(pop)

//Q_DECLARE_METATYPE(app_rx_t);
//Q_DECLARE_METATYPE(app_tx_t);
//Q_DECLARE_METATYPE(app_socket_t);


/* EXPORT PRAR */

class ProtocolFormat{
public:

    Protocol_Format_s   fixHead;
    QVariant    dataVar;

    ProtocolFormat();
    ~ProtocolFormat();
    quint16 get_Protocol_frame_index();
    void setProtocolFrameIndex(quint16 val);

private:
    //quint16         m_Frame_index;


};

#endif

#ifdef __cplusplus
     }
#endif
