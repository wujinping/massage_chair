#include "controller.h"
#include "delay.h"
#include "ble102.h"
/* 此处解析来自用户的命令 */
int controller_handle_user_commands(void *p, char *msg, uint16_t msg_len)
{
    struct controller *ctrl = (struct controller *)p;
    if(!p || !msg || msg_len <=5 ){
	print_err("%s: Invalid parameter\n", __func__);
	return -1;
    }
    /* Authentication command */
    if(!strncmp(msg, "$WAUTN", 5)){
	/* TODO: check the authentication is passed or not */
	char authen_buf[8] = {0};
	strncpy(authen_buf, msg+7, 8);
	/* Now we just ignore that ... */
	ctrl->unlocked = 1;
    }
    /* Start device command */
    else if(!strncmp(msg, "$WSTAT", 5)){
	sscanf((const char *)msg, "$WSTAT,%d*00\r\n", &ctrl->valid_sec);
	queue_callbacks(3,0,ctrl, beeper_work, 0);
    }
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
    ctrler->beeper = para->beeper;
    ctrler->speed_led = para->speed_led;
    ctrler->upper_led1 = para->upper_led1;
    ctrler->upper_led2 = para->upper_led2;
    ctrler->lower_led = para->lower_led;
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
    if(!ctrler->unlocked){
	print_err("%s: Device locked, cannot work!\n", __func__);
	return -1;
    }
    beeper_work(1, 0, ctrler);
    switch(ctrler->ustate)
    {
	case STATE_STATIC:
	    ctrler->ustate = STATE_LOWER;
	    dev->set_range(dev, RANGE_LOWER_HALF);
	    dev->set_speed(dev, ctrler->speed);
	    gpio_set_value(&dev->upper_led1, 0);
	    gpio_set_value(&dev->upper_led2, 1);
	    break;
	case STATE_LOWER:
	    ctrler->ustate = STATE_UPPER;
	    dev->set_range(dev, RANGE_UPPER_HALF);
	    dev->set_speed(dev, ctrler->speed);		
	    gpio_set_value(&dev->upper_led1, 1);
	    gpio_set_value(&dev->upper_led2, 0);
	    break;
	case STATE_UPPER:
	    ctrler->ustate = STATE_ENTIRE;
	    dev->set_range(dev, RANGE_ENTIRE);
	    dev->set_speed(dev, ctrler->speed);		
	    gpio_set_value(&dev->upper_led1, 1);
	    gpio_set_value(&dev->upper_led2, 1);
	    break;
	case STATE_ENTIRE:
	    ctrler->ustate = STATE_STATIC;
	    dev->set_speed(dev, SPEED_STATIC);				
	    gpio_set_value(&dev->upper_led1, 0);
	    gpio_set_value(&dev->upper_led2, 0);
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
    if(!ctrler->unlocked){
	print_err("%s: Device locked, cannot work!\n", __func__);
	return -1;
    }

    beeper_work(1, 0, ctrler);
    switch(ctrler->lstate)
    {
	case STATE_OFF:
	    ctrler->lstate = STATE_ON;
	    dev->set_speed(dev, ctrler->speed);			
	    gpio_set_value(&dev->lower_led, 1);
	    break;
	case STATE_ON:
	    ctrler->lstate = STATE_OFF;
	    dev->set_speed(dev, SPEED_STATIC);			
	    gpio_set_value(&dev->lower_led, 0);
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
    if(!ctrler->unlocked){
	print_err("%s: Device locked, cannot work!\n", __func__);
	return -1;
    }

    beeper_work(1, 0, ctrler);
    switch(ctrler->speed){
	case SPEED_STATIC:
	    ctrler->speed = SPEED_SLOW;
	    bdev->set_speed(bdev, ctrler->speed);	
	    udev->set_speed(udev, ctrler->speed);	
	    gpio_set_value(&dev->speed_led, 0);
	    break;
	case SPEED_SLOW:
	    ctrler->speed = SPEED_MEDIUM;
	    bdev->set_speed(bdev, ctrler->speed);	
	    udev->set_speed(udev, ctrler->speed);			
	    gpio_set_value(&dev->speed_led, 0);
	    break;
	case SPEED_MEDIUM:
	    ctrler->speed = SPEED_FAST;
	    bdev->set_speed(bdev, ctrler->speed);	
	    udev->set_speed(udev, ctrler->speed);			
	    gpio_set_value(&dev->speed_led, 1);
	    break;
	case SPEED_FAST:
	    ctrler->speed = SPEED_STATIC;
	    bdev->set_speed(bdev, ctrler->speed);	
	    udev->set_speed(udev, ctrler->speed);			
	    gpio_set_value(&dev->speed_led, 1);
	    break;
	default:
	    break;
    }
    return 0;
}
int beeper_work(uint8_t count, uint8_t unused, struct controller *ctrler)
{
    uint8_t loop;
    if(!ctrler){
	print_err("%s: Invalid parameter!\n", __func__);
	return -1;
    }
    for(loop = 0; loop < count; loop++){
	gpio_set_value(&ctrler->beeper, 1);
	delay_ms(200);
	gpio_set_value(&ctrler->beeper, 0);
	delay_ms(200)
    }
    return 0;
}
