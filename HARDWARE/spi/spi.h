#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
 	    													  
void spi1_Init(void);			 //��ʼ��SPI1��
void spi1_set_speed(u8 SpeedSet); //����SPI1�ٶ�   
void spi1_write_read(u8 *pdata);//SPI1���߶�дһ���ֽ�
		 
#endif

