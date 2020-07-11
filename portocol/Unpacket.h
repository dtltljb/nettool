	
#ifndef __Unpacket_H
#define __Unpacket_H

#include 	<string.h>
#include 	<stdlib.h>
#include 	<stdint.h>
#include 	<stdio.h>

#include 		"util.h"
#include  	"crc32.h"
#include 		"Protocol_Format.h"

#define		UnpackBufSize				1050	*	Socket_MAX_Frame
#define		unChangeCodeSize			RECEIVE_DATA_MAX_SIZE*2

#define		FRAME_MIN_LENGTH			sizeof(s_Protocol_Format)

#define	Enable_Display_Unpacket_Data				/*define equ enbale display*/

#define	errorCodeUnpackLengthMin		(uint32_t)0x01
#define	errorCodeUnpackVersion			(uint32_t)0x02
#define	errorCodeDataLengthMin			(uint32_t)0x03
#define	errorCodeMagicTail					(uint32_t)0x04
#define	errorCodeCrcCheck						(uint32_t)0x05
#define	errorCodeFrameLengthMin			(uint32_t)0x06
#define	errorCodeMagicHeader				(uint32_t)0x07

uint32_t socket_data_unpack_func(s_Protocol_Format *args, uint8_t *data, uint16_t *len);

#endif
