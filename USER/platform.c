/***************************************************************************//**
 *   @file   platform.c
 *   @brief  platform specific defines and functions, should be fullfilled according to 
 *   the application platform .
*******************************************************************************/
/* */
#include "platform.h"
#include "stm32f10x.h"
#include "controller.h"
#ifdef	STM32F10X_HD

extern struct controller *ctrler;

uint8_t mod_2(uint16_t value)
{
	uint8_t i=0;
	if(!value){
		return 0;
	}
	while(!(value&0x1)){
		i++;
		value >>= 1;
	}
	return i;
}

void gpio_init(struct gpio *pio)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* enable gpio clock */
	RCC_APB2PeriphClockCmd(1 << ((pio->port - GPIOA)/(GPIOB - GPIOA) + 2),ENABLE); 
	/* configure gpio */
	GPIO_InitStructure.GPIO_Pin = pio->pin;
	GPIO_InitStructure.GPIO_Mode = pio->mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(pio->port,&GPIO_InitStructure); 
	
}
void gpio_set_value(struct gpio *pio, u8 value)
{
	GPIO_WriteBit(pio->port, pio->pin, value);
}
u8 gpio_get_value(struct gpio *pio)
{
	return GPIO_ReadInputDataBit(pio->port, pio->pin);
}



void plat_spi_write_read(struct spi_dev *spi, uint8_t *pdata, uint8_t unused)
{
	UNUSED_PARAMETER(index);
	UNUSED_PARAMETER(unused);
	spi_write_read(spi->spi_base, pdata);
}
void platform_spi_init(struct spi_dev *spi, uint32_t freq)
{
	spi_init(spi, freq);
	gpio_init(&spi->ce);
	gpio_init(&spi->nss);
	
}

/* FIXME: platform interrupt initialized here is irrelivant to the interrupt source configured by user. */
char plat_intr_init(struct gpio *pio, EXTITrigger_TypeDef trigger_type)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	gpio_init(pio);

	GPIO_EXTILineConfig((pio->port - GPIOA)/(GPIOB - GPIOA), mod_2(pio->pin));

	EXTI_InitStructure.EXTI_Line = pio->pin;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = trigger_type;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	switch(pio->pin){
	    case 0:
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
		break;
	    case 1:
		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
		break;
	    case 2:
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
		break;
	    case 3:
		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
		break;
	    case 4:
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
		break;
	    case 5:case 6:case 7:case 8:case 9:
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
		break;
	    case 10:case 11:case 12:case 13:case 14:case 15:
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
		break;

	}
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	EXTI_ClearITPendingBit(pio->pin);
	
}
void pwm_init(TIM_TypeDef *tim, uint8_t channel, struct gpio *pio, uint32_t high_pulse)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	switch((uint32_t)tim){
		case (uint32_t)TIM1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  			
			break;
		case (uint32_t)TIM8:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);  			
			break;		
		case (uint32_t)TIM9:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  			
			break;				
		case (uint32_t)TIM10:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);  			
			break;				
		case (uint32_t)TIM11:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);  			
			break;				
		case (uint32_t)TIM15:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15,ENABLE);  			
			break;				
		case (uint32_t)TIM16:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16,ENABLE);  			
			break;					
		case (uint32_t)TIM17:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17,ENABLE);  			
			break;					
		case (uint32_t)TIM2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  			
			break;		
		case (uint32_t)TIM3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  			
			break;			
		case (uint32_t)TIM4:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  			
			break;	
		case (uint32_t)TIM5:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  			
			break;		
		case (uint32_t)TIM6:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  			
			break;
		case (uint32_t)TIM7:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  			
			break;			
		case (uint32_t)TIM12:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);  			
			break;			
		case (uint32_t)TIM13:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);  			
			break;			
		case (uint32_t)TIM14:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  						
			break;
	}
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 	
	gpio_init(pio);
		
	TIM_TimeBaseStructure.TIM_Prescaler= 72 - 1;  
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period= 20000 - 1;  
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(tim,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = high_pulse;
	switch(channel){
		case 1:
			TIM_OC1Init(tim, &TIM_OCInitStructure); 
			TIM_OC1PreloadConfig(tim, TIM_OCPreload_Enable); 		
			break;
		case 2:
			TIM_OC2Init(tim, &TIM_OCInitStructure); 
			TIM_OC2PreloadConfig(tim, TIM_OCPreload_Enable); 		
			break;
		case 3:
			TIM_OC3Init(tim, &TIM_OCInitStructure); 
			TIM_OC3PreloadConfig(tim, TIM_OCPreload_Enable); 		
			break;
		case 4:
			TIM_OC4Init(tim, &TIM_OCInitStructure); 
			TIM_OC4PreloadConfig(tim, TIM_OCPreload_Enable); 		
			break;
		default:
			break;
	}
  TIM_ARRPreloadConfig(tim,ENABLE);	
	TIM_Cmd(tim, ENABLE); 	
}
void pwm_set_high_pulse(TIM_TypeDef *tim, uint8_t channel, uint32_t high_pulse)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = high_pulse;
  TIM_ARRPreloadConfig(tim,DISABLE);	

	switch(channel){
		case 1:
			TIM_OC1Init(tim, &TIM_OCInitStructure); 
			TIM_OC1PreloadConfig(tim, TIM_OCPreload_Enable); 		
			break;
		case 2:
			TIM_OC2Init(tim, &TIM_OCInitStructure); 
			TIM_OC2PreloadConfig(tim, TIM_OCPreload_Enable); 		
			break;
		case 3:
			TIM_OC3Init(tim, &TIM_OCInitStructure); 
			TIM_OC3PreloadConfig(tim, TIM_OCPreload_Enable); 		
			break;
		case 4:
			TIM_OC4Init(tim, &TIM_OCInitStructure); 
			TIM_OC4PreloadConfig(tim, TIM_OCPreload_Enable); 		
			break;
		default:
			break;
	}
  TIM_ARRPreloadConfig(tim,ENABLE);	
	
}
/* CAUTION: irq handler number must be the same as the EXTI souce (i.e. if you connect Px2 as a intr souce, then use EXTI2_IRQHandler instead) */
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line7))
	{
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	if(EXTI_GetITStatus(EXTI_Line9))
	{
		EXTI_ClearITPendingBit(EXTI_Line9);
	}	
	if(EXTI_GetITStatus(EXTI_Line5))
	{
		EXTI_ClearITPendingBit(EXTI_Line5);
	}		
	if(EXTI_GetITStatus(EXTI_Line8))
	{
//		nrf24xx_irq_handler(nrf24l01_rx);		
		EXTI_ClearITPendingBit(EXTI_Line8);
	}	
	
}

void EXTI15_10_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line10))
	{
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
			if(EXTI_GetITStatus(EXTI_Line11))
	{
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
			if(EXTI_GetITStatus(EXTI_Line12))
	{
		ctrler->bdev->upper_edge_reached(ctrler->bdev);
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
			if(EXTI_GetITStatus(EXTI_Line13))
	{
		ctrler->bdev->middle_point_reached(ctrler->bdev);
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
			if(EXTI_GetITStatus(EXTI_Line14))
	{
		ctrler->bdev->lower_edge_reached(ctrler->bdev);
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
			if(EXTI_GetITStatus(EXTI_Line15))
	{
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}
/* FIMXE: FATAL FATAL FATAL , here we just do a solid TIM3 timer init, not a generic TIMER initialization */
int platform_timer_intr_init(uint32_t interval)
{
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 

	TIM_TimeBaseInitStructure.TIM_Prescaler=600-1; 
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period=5000-1;  
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 
	TIM_Cmd(TIM3,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) {
	
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
}
#else

#endif
