#include "transmitter.h"
#include "delay.h"
#include "ble102.h"
/* 此处解析来自用户的命令 */
void controller_handle_user_commands(void *p, char *msg, uint16_t msg_len)
{
}
int controller_init(struct controller **pctrler, struct controller_init_para *para)
{
	int ret;
	struct controller *ctrler = calloc(sizeof(struct controller), 1);
	if(!ctrler){
		print_err("%s: not enough memory!\n", __func__);
		return -1;
	}
	ret = us_motor_init(&ctrler->bdev, para->bpara);
	if(ret < 0){
		print_err("%s: motor(back) initialization failed!\n", __func__);
		return -1;		
	}	
	ret = us_motor_init(&ctrler->udev, para->upara);
	if(ret < 0){
		print_err("%s: motor(under) initialization failed!\n", __func__);
		return -1;		
	}	
	ret = ble10x_dev_init(&ctrler->ble_dev, para->ble10x_init_para);
	if(ret < 0){
		print_err("%s: ble10x module initialization failed!\n", __func__);
		return -1;		
	}	
	ble10x_set_recv_callback(ctrlen->ble_dev, controller_handle_user_commands);
	
	*pctrler = ctrler;
	return 0;
}
