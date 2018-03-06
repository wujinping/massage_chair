
#ifndef _usart_h_
#define _usart_h_

#include "stdio.h"
#include "sys.h" 


void serial_packet_received(void);
void serial_xfer_string(char *str, uint8_t len);
	
void uart3_init(u32 baud_rate);
int usart_init(void);

#endif

