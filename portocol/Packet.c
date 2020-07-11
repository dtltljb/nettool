
#include 			"Packet.h"

/* GLOBAL PRARMETER */

uint16_t	m_Frame_index	=	1;	

/********************************************************************************************************
**@brief		void ChangeDefine(uint8_t *r_data, int *r_len)
**input		:	 uint8_t *r_data, int *r_len
**output	:	 uint8_t *r_data, int *r_len
**	
**	
********************************************************************************************************/

void ChangeDefine(uint8_t *r_data, uint32_t *r_len)
{
	uint32_t d,len;
	uint8_t *ptr,ibuf[ChangeCodeSize];;
	len = *r_len;
	ptr = ibuf;

	memcpy(ptr,r_data,len);
	
	for(d=0;d<len;d++)
	{
		if(*ptr==0x7d)
		{
			*r_data++ = 0x7d;
			*r_data++ = 0x01;
			*r_len += 1;
		}
		else if(*ptr==0xfc)
		{
			*r_data++ = 0x7d;
			*r_data++ = 0x02;
			*r_len += 1;
		}
		else
		{
			*r_data++ = *ptr;
		}
		ptr++;
	}

}

/********************************************************************************************************
**	@brief	 uint32_t socket_data_pack_func(void *args, uint8_t **r_data, uint16_t *r_len)
**	input:	 void *args (NSFPFixHead);
**	output packet result:	 uint8_t *r_data, int *r_len
**	=> data length fill 8 bytes => crc32 => des encypt
********************************************************************************************************/

uint32_t socket_data_pack_func(s_Protocol_Format *args, uint8_t *r_data, uint16_t *r_len)
{
    uint32_t  		Crc32;
    uint8_t			buf[SEND_DATA_MAX_SIZE];

   // uint8_t 		l_arrBcdtime[7] ={0},val;
 
		s_Protocol_Format *fix_head = /*(s_Protocol_Format *)*/args;   
		uint16_t	iLength		=	fix_head->nLength;
		fix_head->MagicHead 	= 	MAGIC_HEAD;
		fix_head->ID		  	=	t_ntohl(fix_head->ID);
		if( fix_head->FrameIndex == 0){
		m_Frame_index++;
		fix_head->FrameIndex  	=	t_ntohl(m_Frame_index); 
		}
		else{
			/*user init FrameIndex*/
		}
		
		fix_head->ProtocolVer 	= 	PROTOCOL_VERSION;
	//	fix_head->nLength	  	=	htons(fix_head->nLength);
		fix_head->nLength	  	=	fix_head->nLength;
		fix_head->MagicTail 	= 	MAGIC_TAIL;
  	
		memcpy(buf, fix_head,PROTOCOL_FIX_LENGTH);						/* copy	frame head section context	*/

		memcpy(&buf[PROTOCOL_FIX_LENGTH], fix_head->pData, iLength);	/* copy data from pData to buf[] */
		
		iLength	+=	PROTOCOL_FIX_LENGTH ;
		Crc32	=	t_ntohl(	crc32(CRC32InitVal, buf,iLength) );
		memcpy(&buf[iLength], (char *)&Crc32, sizeof(Crc32));			/*	copy crc to buf[] */
		
		iLength	+=	sizeof(Crc32) ;
		buf[iLength]	=	fix_head->MagicTail ;
		
		*r_len	=	iLength +	PROTOCOL_Magic_Tail;
		memcpy(r_data,buf,*r_len);										/*	copy data to r_data */
		
		#ifdef Enable_Display_Packet_Data
		printf("send data:\r");
			for(int16_t i=0;i<*r_len;i++)
				printf("%02x ",r_data[i]);
		printf("\r %s,line:%d ,crc32=%4X,*r_len=%d \r\n",__FILE__,__LINE__,Crc32,*r_len);        
		#endif
		
		return	1;
}


/*
	get frame index context 
*/

uint16_t get_Protocol_frame_index(void){
	return	m_Frame_index;
}
