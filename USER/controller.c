#include "controller.h"
#include "delay.h"
#include "ble102.h"
#include "signal.h"
#include "us_motor.h"
int authen_from_user(uint32_t passwd)
{
  return 0;
}
/* 此处解析来自用户的命令 */
int controller_handle_user_commands(void *p, char *msg, uint16_t msg_len)
{
    struct controller *ctrl = (struct controller *)p;
    char *xmit_buf = NULL;
  uint32_t resp = 0, time_to_use = 0;
    if(!p || !msg || msg_len <=5 ){
	dev_err("%s: Invalid parameter\n", __func__);
	return -1;
    }
    xmit_buf = calloc(20, 1);
    /* Request for password seed */
    if(!strncmp(msg, "$R", 2)){
      sprintf(xmit_buf, "$R,%08X*00", ctrl->id_buf[2]);
      queue_callback((unsigned long)xmit_buf,strlen(xmit_buf),(void *)ctrl, (psignal_proc)ctrl->ble_dev->msg_x, 0);
    }
    /* Start device command */
    else if(!strncmp(msg, "$S", 2)){
      sscanf((const char *)msg, "$S,%x,%d*00", &resp, &time_to_use);
      if(authen_from_user(resp) < 0){
        /* Authentication failed */
        sprintf(xmit_buf, "$S,Failed*00");
        queue_callback((unsigned long)xmit_buf,strlen(xmit_buf),(void *)ctrl, (psignal_proc)ctrl->ble_dev->msg_x, 0);        
        return -1;
      }
      else{
        ctrl->unlocked = 1;
        ctrl->valid_sec = time_to_use;
        ctrl->valid_start = platform_get_counter();
        sprintf(xmit_buf, "$S,OK*00");
        queue_callback((unsigned long)xmit_buf,strlen(xmit_buf),(void *)ctrl, (psignal_proc)ctrl->ble_dev->msg_x, 0);          
        queue_callback(3,0,ctrl, (psignal_proc)beeper_work, 0);
      }
    }
    return 0;
}

int controller_usage_verify(struct controller *ctrler)
{
    uint32_t time_of_usage = 0;
    if(!ctrler){
	dev_err("%s: Invalid parameter!\n", __func__);
	return -1;
    }
    if(!ctrler->unlocked){
      return -1;
    }
    time_of_usage = (platform_get_counter() - ctrler->valid_start)/(1000/COUNTER_INTERVAL);
    /* Time of usage already expired */
    if(time_of_usage >= ctrler->valid_sec){
      ctrler->unlocked = 0;
      ctrler->valid_sec = 0;
      ctrler->valid_start = 0;
      ctrler->bdev->stop(ctrler->bdev);
      ctrler->udev->stop(ctrler->udev);
      ctrler->lstate = STATE_OFF;
      ctrler->ustate = STATE_STATIC;
      ctrler->speed = SPEED_STATIC;
      gpio_set_value(&ctrler->speed_led, 0);
      queue_callback(0,0,ctrler, (psignal_proc)beeper_work_long, 0);
      return -1;
    }
    return 0;
      
}

int controller_init(struct controller **pctrler, struct controller_init_para *para)
{
    int ret;
    struct controller *ctrler = calloc(sizeof(struct controller), 1);
    if(!ctrler){
	dev_err("%s: not enough memory!\n", __func__);
	return -1;
    }
    ret = us_motor_init(&ctrler->bdev, para->bpara);
    if(ret < 0){
	dev_err("%s: motor(back) initialization failed!\n", __func__);
	return -1;		
    }	
    ret = us_motor_init(&ctrler->udev, para->upara);
    if(ret < 0){
	dev_err("%s: motor(under) initialization failed!\n", __func__);
	return -1;		
    }	
    ret = ble10x_device_init(&ctrler->ble_dev, para->ble10x_init_para);
    if(ret < 0){
	dev_err("%s: ble10x module initialization failed!\n", __func__);
	return -1;		
    }	
    /* Get the uique CPU ID */
    platform_get_uniq_id(ctrler->id_buf);
    ctrler->lower_start = para->lower_start;
    ctrler->speed_config = para->speed_config;
    ctrler->upper_start_range = para->upper_start_range;
    ctrler->beeper = para->beeper;
    ctrler->speed_led = para->speed_led;
		gpio_init(&ctrler->beeper);
		gpio_init(&ctrler->speed_led);
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
		ctrler->unlocked = 1;
    ctrler->valid_sec = 100;
    ctrler->valid_start = 0;
    *pctrler = ctrler;
    return 0;
}
int controller_back_btn_pressed(struct controller *ctrler)
{
    struct us_motor_device *dev = ctrler->bdev;
    if(!ctrler || !ctrler->bdev){
	dev_err("%s: Invalid parameter!\n", __func__);
	return -1;
    }
    if(!ctrler->unlocked){
	dev_err("%s: Device locked, cannot work!\n", __func__);
	return -1;
    }
    queue_callback(1,0,ctrler, (psignal_proc)beeper_work, 0);
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
      dev->stop(dev);
	    //dev->set_speed(dev, SPEED_STATIC);				
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
	dev_err("%s: Invalid parameter!\n", __func__);
	return -1;
    }
    if(!ctrler->unlocked){
	dev_err("%s: Device locked, cannot work!\n", __func__);
	return -1;
    }

    queue_callback(1,0,ctrler, (psignal_proc)beeper_work, 0);
    switch(ctrler->lstate)
    {
	case STATE_OFF:
	    ctrler->lstate = STATE_ON;
	    dev->set_speed(dev, ctrler->speed);		
      dev->start(dev);
	    break;
	case STATE_ON:
	    ctrler->lstate = STATE_OFF;
	    dev->set_speed(dev, SPEED_STATIC);	
      dev->stop(dev);
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
	dev_err("%s: Invalid parameter!\n", __func__);
	return -1;
    }
    if(!ctrler->unlocked){
	dev_err("%s: Device locked, cannot work!\n", __func__);
	return -1;
    }
    queue_callback(1,0,ctrler, (psignal_proc)beeper_work, 0);
    switch(ctrler->speed){
	case SPEED_STATIC:
	    ctrler->speed = SPEED_SLOW;
	    bdev->set_speed(bdev, ctrler->speed);	
	    udev->set_speed(udev, ctrler->speed);	
	    gpio_set_value(&ctrler->speed_led, 0);
	    break;
	case SPEED_SLOW:
	    ctrler->speed = SPEED_MEDIUM;
	    bdev->set_speed(bdev, ctrler->speed);	
	    udev->set_speed(udev, ctrler->speed);			
	    gpio_set_value(&ctrler->speed_led, 0);
	    break;
	case SPEED_MEDIUM:
	    ctrler->speed = SPEED_FAST;
	    bdev->set_speed(bdev, ctrler->speed);	
	    udev->set_speed(udev, ctrler->speed);			
	    gpio_set_value(&ctrler->speed_led, 1);
	    break;
	case SPEED_FAST:
	    ctrler->speed = SPEED_STATIC;
	    bdev->set_speed(bdev, ctrler->speed);	
	    udev->set_speed(udev, ctrler->speed);			
	    gpio_set_value(&ctrler->speed_led, 1);
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
	dev_err("%s: Invalid parameter!\n", __func__);
	return -1;
    }
    for(loop = 0; loop < count; loop++){
	gpio_set_value(&ctrler->beeper, 1);
	delay_ms(100);
	gpio_set_value(&ctrler->beeper, 0);
	delay_ms(100);
    }
    return 0;
}
int beeper_work_long(uint8_t unused1, uint8_t unused2, struct controller *ctrler)
{
    uint8_t loop;
    if(!ctrler){
	dev_err("%s: Invalid parameter!\n", __func__);
	return -1;
    }
	gpio_set_value(&ctrler->beeper, 1);
	delay_ms(1000);
	gpio_set_value(&ctrler->beeper, 0);
    return 0;
}