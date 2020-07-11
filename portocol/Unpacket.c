
#include 	"Unpacket.h"

//============ public ============================
uint16_t				mReceiveLength = 0;

//------------------	private		--------------


/********************************************************************************************************
**@brief   void PlayFrame(uint8_t *r_data, uint16_t *r_len)
 *
 * @details change code used by this application.
 *
 *	input:uint8_t *r_data, uint16_t *r_len
 *
 *	output:uint8_t *r_data, uint16_t *r_len				!!! NOTE, r_data will rewrite..
 *
 *	return:	0 is success,none is failure
********************************************************************************************************/
uint8_t	PlayFrame(uint8_t *r_data, uint16_t *r_len)
{
	if(*r_len < sizeof(s_Protocol_Format))
		return	errorCodeFrameLengthMin;
	
	uint8_t	buf[unChangeCodeSize];
	uint16_t	len = 0,ilen	=	*r_len;
	s_Protocol_Format 	*fix_head;
	memcpy( buf, r_data, ilen);
	uint8_t 	*ptr = buf,*s_ptr;	
	
	while( fix_head->MagicHead != MAGIC_HEAD ){
		if( (--ilen) < sizeof(s_Protocol_Format)){
			printf("%s,line=%d, search magic head error, length=%d!!\n",__FILE__,__LINE__,ilen);
			return errorCodeMagicHeader;
		}
		fix_head = (s_Protocol_Format*)(++ptr);
	}
	
	s_ptr	=	ptr;						/*	magic head place */
	len += 1;								/*	set magic head length */	
	while( *(++ptr) != MAGIC_TAIL ){
		if( (--ilen) == 0 ){
			printf("%s,line=%d, search magic tail error, length=%d!!\n",__FILE__,__LINE__,ilen);
			return errorCodeMagicTail;
		}
		len += 1;
	}
	
	len += 1;								/*	add magic tail length */
	if ( len < sizeof(s_Protocol_Format) )
		return	errorCodeFrameLengthMin;
	
	/* output context */
	memset( r_data, 0, *r_len);
	memcpy( r_data, s_ptr, len );				/*output frame data context */
	*r_len	=	len;
	return (uint8_t)0;
}

/********************************************************************************************************
**@brief   void UnChangeDefine(uint8_t *r_data, uint16_t *r_len)
 *
 * @details change code used by this application.
 *
 *	input:uint8_t *r_data, uint16_t *r_len
 *
 *	output:uint8_t *r_data, uint16_t *r_len
********************************************************************************************************/

void UnChangeDefine(uint8_t *r_data, uint16_t *r_len)
{
	uint32_t d,len;
	uint8_t *ptr,ibuf[unChangeCodeSize];

	len = *r_len;
	ptr = ibuf;
	memcpy(ptr,r_data,len);

	for(d=0;d<len;d++)
	{
		if(*ptr == 0x7d)
			{
				ptr++;
				switch(*ptr)
				{
					case 0x01:
						*r_data++ = 0x7d;
									*r_len -= 1;
						break;
					case 0x02:
						*r_data++ = 0xfc;
									*r_len -= 1;
						break;
					default:
						
						break;
				}
			}
		else
			{
				*r_data++ = *ptr;
			}
		ptr++;
	}
}

/********************************************************************************************************
**@brief    socket_data_unpack_func
 *
 * @details unpack data used by this application.
 *
 *	input:uint8_t *r_data, int *r_len
 *
 *	output:	 	s_Protocol_Format	*args, frame format context
 *							args->pData,	frame data context
 *	return: 0 is success, non unpack error status code
********************************************************************************************************/


uint32_t socket_data_unpack_func(s_Protocol_Format	*arg, uint8_t *data, uint16_t *r_len)
{

    uint16_t 		ilen, len/*,val16*/;
    uint32_t  		Crc32,val;
    uint8_t			u8;    uint8_t 		*ptr;

	s_Protocol_Format 	*fix_head;
	ilen = *r_len;

    #ifdef Enable_Display_Unpacket_Data 
    printf("%s,line:%d,socket receive data: \n ",__FILE__,__LINE__);
    for(int16_t i=0;i<ilen;i++)
      printf("%02x ",*(data + i) );
    printf("\n %s,line:%d,socket receive length=%d \n ",__FILE__,__LINE__,ilen);
    #endif

    if(ilen < sizeof(s_Protocol_Format)){
        printf("%s,line=%d, errorCodeUnpackLengthMin,length=%d!!\n",__FILE__,__LINE__,ilen);
        return errorCodeUnpackLengthMin;
      }
	  
	/*		search magic header		*/
	ptr	=	data;
	fix_head = (s_Protocol_Format*)ptr;
	while( fix_head->MagicHead != MAGIC_HEAD ){
		if( (--ilen) < sizeof(s_Protocol_Format)){
				printf("%s,line=%d, search magic head error, length=%d!!\n",__FILE__,__LINE__,ilen);
			return errorCodeMagicHeader;
		}
		fix_head = (s_Protocol_Format*)(++ptr);
	}
      
    if(fix_head->ProtocolVer != PROTOCOL_VERSION ){
		printf("%s,line=%d , PROTOCOL_VERSION=%d  error  !!\n",__FILE__,__LINE__,fix_head->ProtocolVer);
        return errorCodeUnpackVersion;
      }
      
    len = htons(fix_head->nLength);
    if(len < FRAME_MIN_LENGTH){
			printf("%s,line=%d , fix_head->nLength=%d smallest !!\n",__FILE__,__LINE__,fix_head->nLength);
    		return errorCodeDataLengthMin;
    	}
		

    u8 = *(ptr + PROTOCOL_FIX_LENGTH + len );			/*get magic tail value*/
	
    if( u8 != MAGIC_TAIL ){
			printf("---%s,line=%d , length error non find magic tail=%x !!\n",__FILE__,__LINE__,u8);
    		return errorCodeMagicTail;
    	}

	/*   check  crc32  */
	Crc32=crc32(CRC32InitVal, ptr, PROTOCOL_FIX_LENGTH + len );							/* from magic head to crc  */
	
	/*  get crc value  */
    val = *(ptr +  PROTOCOL_FIX_LENGTH + len   );
	val <<= 8;
    val |= *(ptr + PROTOCOL_FIX_LENGTH + len  + 1 );
	val <<= 8;
    val |= *(ptr + PROTOCOL_FIX_LENGTH + len  + 2 );
    val <<= 8;
    val |= *(ptr + PROTOCOL_FIX_LENGTH + len  + 3 );

    if(Crc32 != val){
        printf("%s,line=%d ,ilen=%d,Crc32=%#8x\n",__FILE__,__LINE__,ilen,Crc32);
        return	errorCodeCrcCheck;
    }

	/* copy data to *arg*/
	arg->MagicHead		=	fix_head->MagicHead;
	arg->ID				=	fix_head->ID;
	arg->FrameIndex	=	fix_head->FrameIndex;
	arg->FrameType	=	fix_head->FrameType;
	arg->ProtocolVer	=	fix_head->ProtocolVer;	
	arg->FrameTotal	=	fix_head->FrameTotal;
	arg->FrameSequ	=	fix_head->FrameSequ;
	arg->nLength	=	fix_head->nLength;
	memcpy(arg->pData, ptr + PROTOCOL_FIX_LENGTH, len);		/*	copy data to arg->pData, note pionter RAM SPACE */
	arg->crc		=	fix_head->crc;
	arg->MagicTail	=	fix_head->MagicTail;
	return	(uint32_t)0;
}
