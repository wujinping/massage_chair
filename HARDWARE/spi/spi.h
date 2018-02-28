#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
 	    													  
void spi1_Init(void);			 //初始化SPI1口
void spi1_set_speed(u8 SpeedSet); //设置SPI1速度   
void spi1_write_read(u8 *pdata);//SPI1总线读写一个字节
		 
#endif

