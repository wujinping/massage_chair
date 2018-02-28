#include "stm32_config.h"
#include "led.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "nrf24xx.h"
#include "transmitter.h"
#include "ds311x.h"
#include "signal.h"
struct nrf24xx_init_para nrf_para = {
	/* Index of SPI device being used */
	SPI1,
	/* SPI SCLK pin */
	{GPIOA, GPIO_Pin_5, GPIO_Mode_AF_PP},
	/* SPI MISO pin */
	{GPIOA, GPIO_Pin_6, GPIO_Mode_AF_PP},
	/* SPI MOSI pin */
	{GPIOA, GPIO_Pin_7, GPIO_Mode_AF_PP},
	/* SPI CS pin */
	{GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP},
	/* SPI NSS pin*/
	{GPIOC, GPIO_Pin_4, GPIO_Mode_Out_PP},	
	/* SPI IRQ pin */
	{GPIOC, GPIO_Pin_5, GPIO_Mode_IPU},
	/* nRF24L01 work mode */
	MODE_RX,
	/* nRF24L01 payload length */
	RX_PLOAD_WIDTH,
	/* nRF24L01 Module Index (TX device should be 0, RX device should be 1-10)*/
	2
};
struct ds311x_init_para ds311x_para_h = {
	DS3118,
	TIM4,
	3,
	{GPIOB, GPIO_Pin_8, GPIO_Mode_AF_PP},	
	0
};
struct ds311x_init_para ds311x_para_v = {
	DS3115,
	TIM4,
	4,
	{GPIOB, GPIO_Pin_9, GPIO_Mode_AF_PP},	
	90
};
struct controller_init_para para = {
	CTRLER_RECEIVER,
	&nrf_para,
	&ds311x_para_h,
	&ds311x_para_v
};

struct controller *ctrler;

void hardware_failure()
{
	u32 i = 0;	
	while (1)
	{
		i++;
		if(i == 300000)
		{
			i = 0;
			LED1 = !LED1;	
		}
	}	
}
int main(void)
{
	u32 i = 0;
	int ret;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	delay_init(72);	
	USARTx_Init(9600);
	LED_Init();	
	delay_ms(100);
	init_signal_queue();	
	ret = controller_init(&ctrler, &para);
	if(ret < 0){
		hardware_failure();
	}
	while (1)
	{
		check_signal();
		if(++i == 1000000){
			i = 0;
			LED0 = !LED0;	
		}
	}
}

