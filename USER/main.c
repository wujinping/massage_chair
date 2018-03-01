#include "stm32_config.h"
#include "led.h"
#include "hw_config.h"
#include "transmitter.h"
#include "signal.h"
#include "us_motor.h"

/* Initial parameters for the motor of the back */
struct us_motor_init_para bmotor = {
    MOTOR_FOR_BACK,
    TIM1,
    1,
    {GPIOA, GPIO_Pin_8, GPIO_Mode_Out_PP},
    {GPIOA, GPIO_Pin_10, GPIO_Mode_Out_PP},
    {GPIOA, GPIO_Pin_11, GPIO_Mode_Out_PP},
    {GPIOB, GPIO_Pin_12, GPIO_Mode_IPD},
    {GPIOB, GPIO_Pin_13, GPIO_Mode_IPD},
    {GPIOB, GPIO_Pin_14, GPIO_Mode_IPD},
    SPEED_STATIC,
    DIR_FORWARD,
    RANGE_UPPER_HALF
};
/* Initial parameters for the motor of the under */
struct us_motor_init_para umotor = {
    MOTOR_FOR_UNDER,
    TIM1,
    2,
    {GPIOA, GPIO_Pin_9, GPIO_Mode_Out_PP},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    SPEED_STATIC,
    0,
    0
};
struct controller_init_para para = {
	&bmotor,
	&umotor
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
	}
}

