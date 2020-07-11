#if defined(__cplusplus)
 extern "C" {
#endif

#ifndef __PROTOCOL_FORMAT_H
#define __PROTOCOL_FORMAT_H

	 #include <stdint.h>
	 #include <stdbool.h>
	 
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
#define		MAGIC_HEAD					0xFC
#define		MAGIC_TAIL					0xFC
#define		PROTOCOL_VERSION			0x01

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
                 uint16_t		nLength;			/* 	receive data length */
								uint8_t			*pData;					/*	Data area pointer	*/
								uint32_t		crc;
								uint8_t			MagicTail;				/*	0xFC	*/
}__attribute__((packed))s_Protocol_Format;

#pragma pack(pop)

#define		PROTOCOL_pData_LEN		sizeof(uint8_t*)
#define		PROTOCOL_Data_CRC		sizeof(uint32_t)
#define		PROTOCOL_Magic_Tail		sizeof(uint8_t)
#define		PROTOCOL_FIX_LENGTH		sizeof(s_Protocol_Format) - PROTOCOL_pData_LEN \
									- PROTOCOL_Data_CRC - PROTOCOL_Magic_Tail

/* MARCO DEFINE */
#define	APP_FRAME_COUNT		5											/* 应用层分包数,最多5包 */
#define	FRAME_MAX_SIZE		1050									/* socket 帧 最大数据长度*/

#define	RECEIVE_DATA_MAX_SIZE	FRAME_MAX_SIZE
#define	SEND_DATA_MAX_SIZE		FRAME_MAX_SIZE

#define	APP_DATA_MAX_SIZE			FRAME_MAX_SIZE*APP_FRAME_COUNT		/* 应用层数据最大长度 */

#ifndef FRAME_BUFFER_SIZE
    #define FRAME_BUFFER_SIZE    (FRAME_MAX_SIZE - sizeof(s_Protocol_Format))
#endif

		
/* Received message object */
typedef struct{
	uint16_t  		FrameIndex;					/*	receive frame index */
	uint8_t			FrameType;					/* 	receive frame type */
  uint16_t			FrameTotal;					/* 	Mul frame total */
  uint16_t			FrameSequ;					/*	Mul frame sequce */
  uint16_t			nLength;						/* 	LLC receive data length */
	uint8_t       data[FRAME_BUFFER_SIZE];	/*  receive buffer */
//    void               *object;										/* pionter sdo server entry  */
  void              (*pFunct)(void *object, void *message); 		/* message pionter receive data entry */
}CO_rx_t;


/* Transmit message object. */
typedef struct{
	uint16_t  		FrameIndex;							/*	receive frame index */
	uint8_t			  FrameType;							/* 	receive frame type */
  uint16_t			FrameTotal;							/* 	Mul frame total */
  uint16_t			FrameSequ;							/*	Mul frame sequce */
  uint16_t			nLength;							/* 	LLC receive data length */
  uint8_t       *data;					/* transmit buffer */
	volatile uint16_t		Length;							/*	application layer data length */
	volatile uint8_t    bufferFull;
  volatile uint8_t    syncFlag;
}CO_tx_t;															/* ALIGN_STRUCT_DWORD; */


/* socket module object. */
typedef struct{
	uint32_t           	ident;
	int32_t 			socket;			/* socket 描述符  */
    CO_rx_t         *rxArray;
    uint16_t            rxSize;
    CO_tx_t         *txArray;
    uint16_t            txSize;
	uint16_t			rxBufferOffset;
	uint16_t			txBufferOffset;
	uint16_t			sequence;
    volatile uint8_t    rxArrayFull;
    uint32_t            errOld;		/* crc error etc.. */
    void               *em;
}CO_socket_t;


/* EXPORT PRAR */
extern	uint16_t	m_Frame_index;



#endif

#ifdef __cplusplus
     }
#endif
