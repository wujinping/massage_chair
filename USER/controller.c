#include "controller.h"
#include "delay.h"
#include "ble102.h"
/* 此处解析来自用户的命令 */
int controller_handle_user_commands(void *p, char *msg, uint16_t msg_len)
{
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
	/* Initialize KEY interrupts */
	plat_intr_init(&ctrler->lower_start, EXTI_Trigger_Falling);
	plat_intr_init(&ctrler->upper_start_range, EXTI_Trigger_Falling);
	plat_intr_init(&ctrler->speed_config, EXTI_Trigger_Falling);
	
	ctrler->back_key_pressed = controller_back_btn_pressed;
	ctrler->bottom_key_pressed = controller_bottom_btn_pressed;
	ctrler->speed_key_pressed = controller_speed_btn_pressed;
	
	ctrler->ustate = STATE_STATIC;
	ctrler->lstate = STATE_OFF;
	ctrler->speed = SPEED_STATIC;
	*pctrler = ctrler;
	return 0;
}
int controller_back_btn_pressed(struct controller *ctrler)
{
	struct us_motor_device *dev = ctrler->udev;
		if(!ctrler || !ctrler->bdev){
		print_err("%s: Invalid parameter!\n", __func__);
		return -1;
	}
	switch(ctrler->ustate)
	{
		case STATE_STATIC:
			ctrler->ustate = STATE_LOWER;
		dev->set_range(dev, RANGE_LOWER_HALF);
		dev->set_speed(dev, ctrler->speed);
			break;
		case STATE_LOWER:
			ctrler->ustate = STATE_UPPER;
			dev->set_range(dev, RANGE_UPPER_HALF);
			dev->set_speed(dev, ctrler->speed);		
			break;
		case STATE_UPPER:
			ctrler->ustate = STATE_ENTIRE;
		dev->set_range(dev, RANGE_ENTIRE);
		dev->set_speed(dev, ctrler->speed);		
			break;
		case STATE_ENTIRE:
			ctrler->ustate = STATE_STATIC;
		dev->set_speed(dev, SPEED_STATIC);				
			break;	
		default:
			break;
	}
	return 0;
}
int controller_bottom_btn_pressed(struct controller *ctrler)
{
	struct us_motor_device *dev = ctrler->udev;	
	if(!ctrler || !ctrler->udev){
		print_err("%s: Invalid parameter!\n", __func__);
		return -1;
	}

		switch(ctrler->lstate)
	{
		case STATE_OFF:
			ctrler->lstate = STATE_ON;
			dev->set_speed(dev, ctrler->speed);			
			break;
		case STATE_ON:
			ctrler->lstate = STATE_OFF;
			dev->set_speed(dev, SPEED_STATIC);			
			break;
		default:
			break;
	}
		return 0;
}
int controller_speed_btn_pressed(struct controller *ctrler)
{
	struct us_motor_device *bdev = ctrler->bdev;
	struct us_motor_device *udev = ctrler->udev;	
		if(!ctrler || !ctrler->udev || !ctrler->bdev){
		print_err("%s: Invalid parameter!\n", __func__);
		return -1;
	}

	switch(ctrler->speed){
		case SPEED_STATIC:
			ctrler->speed = SPEED_SLOW;
		bdev->set_speed(bdev, ctrler->speed);	
		udev->set_speed(udev, ctrler->speed);	
			break;
		case SPEED_SLOW:
			ctrler->speed = SPEED_MEDIUM;
		bdev->set_speed(bdev, ctrler->speed);	
		udev->set_speed(udev, ctrler->speed);			
			break;
		case SPEED_MEDIUM:
			ctrler->speed = SPEED_FAST;
		bdev->set_speed(bdev, ctrler->speed);	
		udev->set_speed(udev, ctrler->speed);			
			break;
		case SPEED_FAST:
			ctrler->speed = SPEED_STATIC;
		bdev->set_speed(bdev, ctrler->speed);	
		udev->set_speed(udev, ctrler->speed);			
			break;
		default:
			break;
	}
	return 0;
}
