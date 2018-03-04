
#include "us_motor.h"

long speed_to_high_pulse(struct us_motor_device *umd, enum motor_speed speed)
{
	long high_pulse = 0;	

	switch(speed) {
	    case SPEED_STATIC:
		break;
	    case SPEED_SLOW:
		break;
	    case SPEED_MEDIUM:
		break;
	    case SPEED_FAST:
		break;
	    default:
		break;
	}
/* 2018-02-28 15:41 wujinping@toec Ltd. 
	high_pulse = 500 +  ((2500 - 500)*angle)/ds311x->max_angle;
 */
	return high_pulse;
}

int set_dir(struct us_motor_device *umd, enum motion_direction dir)
{
    struct gpio *pdir1 = &(umd->dir1);
	struct gpio *pdir2 = &(umd->dir2);
    if(!umd || !pdir1 || !pdir2){
	dev_err("%s:invalid argument.\n", __func__);
	return (int)-1;
    }
    umd->dir = dir;

}
int set_range(struct us_motor_device *umd, enum motion_range range)
{
    if(!umd){
	dev_err("%s:invalid argument.\n", __func__);
	return (int)-1;
    }
    umd->range = range;
}
int set_speed(struct us_motor_device *umd, enum motor_speed speed)
{
    long high_pulse = speed_to_high_pulse(umd, speed);
    if(!umd || (high_pulse < 0)){
	print_err("%s: invalid parameter\n", __func__);
	return -1;
    }
    umd->speed = speed;
    pwm_set_high_pulse(umd->tim, umd->channel, high_pulse);
    return 0;
}
int us_motor_init(struct us_motor_device **pum, struct us_motor_init_para *para)
{
	struct us_motor_device *umd = calloc(sizeof(struct us_motor_device), 1);	
	if(!pum || !para)
		return -1;
	if(!umd){
		print_err("%s: not enough memory\n", __func__);
		return -1;
	}
	umd->power = para->power;
	umd->tim = para->tim;
	umd->channel = para->channel;
	umd->type = para->type;
	if(MOTOR_FOR_BACK == umd->type){
	    umd->dir1 = para->dir1;
	    umd->dir2 = para->dir2;
	    umd->upper_edge= para->upper_edge;
	    umd->middle_pos= para->middle_pos;
	    umd->lower_edge= para->lower_edge;
	    umd->upper_edge_reached = us_motor_upper_edge_reached;
	    umd->middle_point_reached = us_motor_middle_point_reached;
	    umd->lower_edge_reached = us_motor_lower_edge_reached;
	}

	*pum = umd;	
	/*  TODO: 修改speed对应PWM的high-pulse时间, 并且修改PWM的设置 */
	pwm_init(umd->tim, umd->channel, &umd->power,speed_to_high_pulse(umd, para->default_speed));

	/* TODO: IO口初始化是否正确 */
	if(MOTOR_FOR_BACK == umd->type){
	    gpio_init(&umd->dir1);
	    gpio_init(&umd->dir2);
	    gpio_init(&umd->upper_edge);
	    gpio_init(&umd->middle_pos);
	    gpio_init(&umd->lower_edge);
	}
	/* TODO: 增加限位检测输入点的中断初始化处理 */
	if(MOTOR_FOR_BACK == umd->type){
	    plat_intr_init(&umd->upper_edge, EXTI_Trigger_Rising);
	    plat_intr_init(&umd->middle_pos, EXTI_Trigger_Rising);
	    plat_intr_init(&umd->lower_edge, EXTI_Trigger_Rising);
	}
	dev_info("%s:init succeeded.\n", __func__);
	delay_ms(100);
	
	return 0;
}
int us_motor_start(struct us_motor_device *umd)
{
    struct gpio *pio = &(umd->power);
    if(!umd || !pio){
	dev_err("%s:invalid argument.\n", __func__);
	return (int)-1;
    }
    set_speed(umd, umd->speed);
    return 0;
}
int us_motor_stop(struct us_motor_device *umd)
{
    struct gpio *pio = &(umd->power);
    if(!umd || !pio){
	dev_err("%s:invalid argument.\n", __func__);
	return (int)-1;
    }
    set_speed(umd, SPEED_STATIC);
    return 0;
}

void us_motor_upper_edge_reached(struct us_motor_device *umd)
{
    if(!umd || (MOTOR_FOR_BACK != umd->type)){
	dev_err("%s:invalid argument.\n", __func__);
    }
    switch(umd->range){
	case RANGE_UPPER_HALF:
	    break;
	case RANGE_LOWER_HALF:
	    break;
	case RANGE_ENTIRE:
	    break;
	default:
	    break;
    }
}
void us_motor_middle_point_reached(struct us_motor_device *umd)
{
    if(!umd || (MOTOR_FOR_BACK != umd->type)){
	dev_err("%s:invalid argument.\n", __func__);
	return;
    }
    switch(umd->range){
	case RANGE_UPPER_HALF:
	    break;
	case RANGE_LOWER_HALF:
	    break;
	case RANGE_ENTIRE:
	    break;
	default:
	    break;
    }
}
void us_motor_lower_edge_reached(struct us_motor_device *umd)
{
    if(!umd || (MOTOR_FOR_BACK != umd->type)){
	dev_err("%s:invalid argument.\n", __func__);
    }
    switch(umd->range){
	case RANGE_UPPER_HALF:
	    break;
	case RANGE_LOWER_HALF:
	    break;
	case RANGE_ENTIRE:
	    break;
	default:
	    break;
    }
}
