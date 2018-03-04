#include "controller.h"
#include "delay.h"
#include "ble102.h"
/* 此处解析来自用户的命令 */
int controller_handle_user_commands(void *p, char *msg, uint16_t msg_len)
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
	ret = ble10x_device_init(&ctrler->ble_dev, para->ble10x_init_para);
	if(ret < 0){
		print_err("%s: ble10x module initialization failed!\n", __func__);
		return -1;		
	}	
	ctrler->lower_start = para->lower_start;
	ctrler->speed_config = para->speed_config;
	ctrler->upper_start_range = para->upper_start_range;
	ble10x_set_recv_callback(ctrler->ble_dev, controller_handle_user_commands);
	
	*pctrler = ctrler;
	return 0;
}
