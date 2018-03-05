
#include "delay.h"

#if _USER_SysTick	


static uint8_t  fac_us=0;	
static uint16_t fac_ms=0;	

void delay_init(uint8_t SYSCLK)
{
	SysTick->CTRL &= 0xfffffffb;	
	fac_us = SYSCLK/8;
	fac_ms = (uint16_t)fac_us*1000;	
}
							   
void delay_us(uint32_t nus)
{		
	uint32_t temp;
		    	 
	SysTick->LOAD = nus*fac_us; 
	SysTick->VAL = 0x00;  
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   
	do
	{
		temp = SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	
	SysTick->VAL = 0x00;	
}

void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;
			   
	SysTick->LOAD = (uint32_t)nms*fac_ms;	
	SysTick->VAL = 0x00;          
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   
	do
	{
		temp = SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));	
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL = 0x00;
}


#else	



static uint32_t CPU_CLK;

void delay_init(uint8_t SYSCLK)
{
	CPU_CLK = (uint32_t)SYSCLK*1000*1000;	

	DEM_CTRL |= 1<<24;	

	DWT_CTRL |= 1<<0;	
}

void delay_us(uint32_t nus)
{
	uint32_t savecount,endcnt,CPU_cnt;

	savecount = DWT_CYCCNT;	
	CPU_cnt = nus*(CPU_CLK/(1000*1000));

	endcnt = savecount + CPU_cnt;	
	
	if(endcnt > savecount)	
	{
		while(DWT_CYCCNT < endcnt);
	}
	else	
	{
		while(DWT_CYCCNT > endcnt);
		while(DWT_CYCCNT < endcnt);
	}
}

#endif

