
#include "usart.h"
#include "controller.h"

/* FIXME: global variables trap, but we don't have much time */
char serial_buf[50] = {0};
char serial_count = 0;

extern struct controller *ctrler;
#if 1
#pragma import(__use_no_semihosting)                          
struct __FILE 
{ 
    int handle; 

}; 

FILE __stdout;         
_sys_exit(int x) 
{ 
    x = x; 
} 

int fputc(int ch, FILE *f)
{      
    while((USART1->SR & 0x40) == 0);	
    USART1->DR = (uint8_t)ch;	
    return ch;
}
#endif

void USARTx_Init(uint32_t bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    USART1->CR1 |= 1 << 8;	
    USART1->CR1 |= 1 << 5;

    MY_NVIC_Init(3, 3, USART1_IRQn, NVIC_PriorityGroup_2);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    USART_DeInit(USART1); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);                   
}
void uart3_init(u32 baud_rate)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
    GPIO_Init(GPIOB,&GPIO_InitStructure); 

    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    USART_ClearFlag(USART3, USART_FLAG_TC);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
    NVIC_Init(&NVIC_InitStructure);
    USART_Cmd(USART3, ENABLE);
}
void USART1_IRQHandler(void)
{
}

void serial_packet_received()
{
    ctrler->ble_dev->usx->recv_cb(ctrler->ble_dev, serial_buf, serial_count);
    /* clear current buffer and counter */
    memset(serial_buf, 0, sizeof(serial_buf));
    serial_count = 0;
}
void USART3_IRQHandler(void)                	
{
    u8 recv_char;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
    {
	/* TODO: clear the TIMER counter when a new char is received */
	platform_timer_refresh();
	recv_char =USART_ReceiveData(USART3);
	if(serial_count < 50){
	    serial_buf[serial_count++] = recv_char;
	}
	else{
	    memset(serial_buf, 0, sizeof(serial_buf));
	    serial_count = 0;
	}
    } 
} 


