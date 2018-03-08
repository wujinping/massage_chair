#include "stm32_config.h"
#include "hw_config.h"
#include "controller.h"
#include "signal.h"
#include "us_motor.h"
#include "ble102.h"

struct controller *ctrler;
/* Initial parameters for the motor of the back */
struct us_motor_init_para bmotor_para = {
    MOTOR_FOR_BACK,
    TIM1,
    1,
    {GPIOA, GPIO_Pin_8, GPIO_Mode_Out_PP},
    {GPIOA, GPIO_Pin_10, GPIO_Mode_Out_PP},
    {GPIOA, GPIO_Pin_11, GPIO_Mode_Out_PP},
    
    {GPIOB, GPIO_Pin_8, GPIO_Mode_Out_PP},
    {GPIOB, GPIO_Pin_9, GPIO_Mode_Out_PP},
    {0,0,0},
    
    {GPIOB, GPIO_Pin_12, GPIO_Mode_IPD},
    {GPIOB, GPIO_Pin_3, GPIO_Mode_IPD},
    {GPIOB, GPIO_Pin_14, GPIO_Mode_IPD},
    SPEED_STATIC,
    DIR_FORWARD,
    RANGE_UPPER_HALF
};
/* Initial parameters for the motor of the under */
struct us_motor_init_para umotor_para = {
    MOTOR_FOR_LEG,
    TIM1,
    2,
    {GPIOA, GPIO_Pin_9, GPIO_Mode_Out_PP},
    {0,0,0},
    {0,0,0},
    
    {0,0,0},
    {0,0,0},
    {GPIOC, GPIO_Pin_14, GPIO_Mode_Out_PP},  
    
    {0,0,0},
    {0,0,0},    
    {0,0,0},
    SPEED_STATIC,
    0,
    0
};

/* Initial parameters for ble10x */    
struct ble10x_init_para ble10x_para = {
    USART3,
    {GPIOA, GPIO_Pin_6, GPIO_Mode_Out_PP},
    {GPIOA, GPIO_Pin_5, GPIO_Mode_Out_PP},
    {GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP},	
    57600,
    MODE_SLAVE,
    0
};
/* Initial parameters for the controller */
struct controller_init_para para = {
    {GPIOC, GPIO_Pin_13, GPIO_Mode_IPU},
    {GPIOC, GPIO_Pin_15, GPIO_Mode_IPU},
    {GPIOA, GPIO_Pin_1, GPIO_Mode_IPU},
    /* FIXME: PB5 is actually used as beeper output, but something terrible happens when beeper beeps */
    {GPIOB, GPIO_Pin_4, GPIO_Mode_Out_PP},

    {GPIOA, GPIO_Pin_0, GPIO_Mode_Out_PP},
    &bmotor_para,
    &umotor_para,
    &ble10x_para
};

int main(void)
{
    int ret;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
    delay_init(8);	
    delay_ms(100);
    init_signal_queue();	
    /* Initialize platform counter (for available time management)*/
    platform_counter_init(COUNTER_INTERVAL);
    /* Initialize the controller */
    ret = controller_init(&ctrler, &para);
    if(ret < 0){
      /* TODO: blink a LED or something to indicate hardware failure */
    }
    while (1)
    {
      poll_callbacks();
      controller_usage_verify(ctrler);
    }
}

