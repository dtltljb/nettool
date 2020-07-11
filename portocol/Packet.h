

/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(_PACKET_H)
#define _PACKET_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


#include 			"Protocol_Format.h"
#include 			"crc32.h"
#include 			"util.h"
#include 			"util_date.h"

#define ChangeCodeSize      SEND_DATA_MAX_SIZE*2   //send data + frame head << ChangeCodeSize

//#define	Enable_Display_Packet_Data				/*define equ enbale display*/

//===================

uint32_t socket_data_pack_func(s_Protocol_Format *args, uint8_t *r_data, uint16_t *r_len);

uint16_t get_Protocol_frame_index(void);

#endif
#ifdef __cplusplus
     }
#endif
