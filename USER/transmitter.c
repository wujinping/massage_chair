#include "nrf24xx.h"
#include "transmitter.h"
#include "ds311x.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "delay.h"
extern struct nrf24xx *nrf24l01_tx;
extern struct controller *ctrler;
struct xmit_list *head = NULL;

void xmit_list_push(u8 *packet, u16 packet_len)
{
    /* no items in the list */
    struct xmit_list *p = head;
		struct xmit_list *item = calloc(sizeof(struct xmit_list), 1);
		item->packet = packet;
		item->packet_len = packet_len;
    if(!head){
			head = item;
    }
    else{
			while(NULL != p->next){
				p = p->next;
			}
			p->next = item;
			item->next = NULL;
		}	
}
/* pop head */
struct xmit_list * xmit_list_pop()
{
    struct xmit_list *p = head;
		if(NULL == p){
			return NULL;
		}
		else{
			head = head->next;
		}
		return p;
}
/* Handle commands comming from USB */
void transmitter_handle_upstream(uint8_t *pup, uint8_t len)
{
	uint32_t index, h_angle, v_angle;
	uint8_t *presp = NULL;
	uint8_t *ptx = NULL;
	if(!pup || !len){
		return;
	}
	if((pup[0] != '$') || (pup[len - 1] != '\n')){
		presp = calloc(32, 1);
		strcpy((char *)presp, "$WICTL,INVALID FORMAT*00\r\n");
		xmit_list_push(presp, strlen((const char *)presp));				
		return;
	}
	if(len > RX_PLOAD_WIDTH){
		presp = calloc(32, 1);
		strcpy((char *)presp, "$WICTL,OVER LENGTH*00\r\n");
		xmit_list_push(presp, strlen((const char *)presp));						
		return;
	}
	sscanf(pup+6, ",%d,", &index);
	if(index > ADDR_MAX_NUM){
		presp = calloc(32, 1);
		strcpy((char *)presp, "$WICTL,INVALID INDEX*00\r\n");
		xmit_list_push(presp, strlen((const char *)presp));								
		return;
	}
	ptx = calloc(RX_PLOAD_WIDTH, 1);
	strcpy(ptx, pup);
	nrf24xx_tx_packet(ctrler->nrf, index, ptx);
	free(ptx);	

}
/* Handle interrupt triggered by nRF24xx transceiver */
void transmitter_irq_handler(struct controller *ctrler)
{
	int ret;
	uint8_t *presp = NULL;
	ret = nrf24xx_irq_handler(ctrler->nrf);
	if(CTRLER_TRANSMITTER == ctrler->controller_type){
		presp = calloc(16, 1);
		if(ret < 0){
			strcpy((char *)presp, "$WICTL,FAIL*00\r\n");
		}
		else{
			strcpy((char *)presp, "$WICTL,OK*00\r\n");		
		}
		xmit_list_push(presp, strlen((const char *)presp));
	}	
	/* leave the transmitter_handle_downstream() function to respond the command */
	else{
	}
}
/* Handle commands received from nRF24xx */
void transmitter_handle_downstream(uint8_t *pdown, uint8_t len)
{
	uint32_t index, h_angle, v_angle;
	/* Set Absolute Angle */
	if(!strncmp(pdown+1, "WISAA", 5)){
		sscanf((const char *)pdown, "$WISAA,%d,%d,%d*00\r\n", &index, &h_angle, &v_angle);
		if(h_angle > DS3118_ANGLE_MAX || v_angle > DS3115_ANGLE_MAX){
			print_err("%s: turning angle out of range.\n", __func__);
		}	
		else{
			if(ctrler->nrf->module_index == index){
				add_signal(h_angle, 20, ctrler->ds311x_h, ds311x_set_angle_delayed, 0);
				add_signal(v_angle, 0, ctrler->ds311x_v, ds311x_set_angle_delayed, 0);	
			}				
		}
	}	
	/* Set Relative Angle*/
	else if(!strncmp(pdown+1, "WISAR", 5)){
			sscanf((const char *)pdown, "$WISAR,%d,%d,%d*00\r\n", &index, &h_angle, &v_angle);		
			if(ctrler->nrf->module_index == index){
				add_signal(h_angle, 20, ctrler->ds311x_h, ds311x_set_angle_relative_delayed, 0);
				add_signal(v_angle, 0, ctrler->ds311x_v, ds311x_set_angle_relative_delayed, 0);		
			}
	}
	/* Unmaned Aeiral Vehical control commands */
	else if(!strncmp(pdown+1, "WIAIR", 5)){
		
	}
	else{
	}
	free(pdown);
}
void USB_Port_Set(u8 usb_sw)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    //使能PORTA时钟
		   	 
	if(usb_sw)	_SetCNTR(_GetCNTR()&(~(1<<1)));//退出断电模式
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
		GPIOA->CRH &= 0XFFF00FFF;
		GPIOA->CRH |= 0X00033000;
		PAout(12) = 0;	    		  
	}
}
int usb_serial_init()
{
	delay_ms(300);
 	USB_Port_Set(0);
	delay_ms(300);
	USB_Port_Set(1);

	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	return 0;
}
int controller_init(struct controller **pctrler, struct controller_init_para *para)
{
	int ret;
	struct controller *ctrler = calloc(sizeof(struct controller), 1);
	if(!ctrler){
		print_err("%s: not enough memory!\n", __func__);
		return -1;
	}
	ctrler->controller_type = para->type;	
	if(CTRLER_TRANSMITTER == ctrler->controller_type){
		usb_serial_init();
	}
	else{
		ret = ds311x_init(&ctrler->ds311x_h, para->ds311x_para_h);
		if(ret < 0){
			print_err("%s: horizental ds311x cradle initialize failed!\n", __func__);
			return -1;		
		}		
		ret = ds311x_init(&ctrler->ds311x_v, para->ds311x_para_v);
		if(ret < 0){
			print_err("%s: vertical ds311x cradle initialize failed!\n", __func__);
			return -1;		
		}		
	}
	ret = nrf24xx_init(&ctrler->nrf, para->nrf_para);
	if(ret < 0){
		print_err("%s: nrf24xx wirless transceiver initialize failed!\n", __func__);
		return -1;		
	}	
	
	*pctrler = ctrler;
	return 0;
}
