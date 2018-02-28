
#include "us_motor.h"

long speed_to_high_pulse(struct us_motor_device *umd, enum motor_speed speed)
{
	long high_pulse = 0;	
	if(speed <  || angle > ds311x->max_angle){
		return -1;
	}
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
int us_motor_init(struct us_motor_device **pum, struct us_motor_init_para *para);
{
	struct us_motor_device *umd = calloc(sizeof(struct us_motor_device), 1);	
	if(!pum || !para)
		return -1;
	if(!umd){
		print_err("%s: not enough memory\n", __func__);
		return -1;
	}
	umd->pwm = para->pwm;
	umd->tim = para->tim;
	umd->channel = para->channel;
	umd->type = para->type;
	umd->dir = para->dir;
	umd->upper_edge= para->upper_edge;
	umd->middle_point= para->middle_point;
	umd->lower_edge= para->lower_edge;
	
	umd->upper_edge_reached = us_motor_upper_edge_reached;
	umd->middle_point_reached = us_motor_middle_point_reached;
	umd->lower_edge_reached = us_motor_lower_edge_reached;

	*pum = umd;	
	/*  TODO: 修改speed对应PWM的high-pulse时间, 并且修改PWM的设置 */
	pwm_init(umd->tim, umd->channel, &umd->pwm, speed_to_high_pulse(para->default_speed));
	delay_ms(100);
	
	return 0;
}

int us_motor_set_speed(struct us_motor_device *umd, enum speed_rank speed)
{
	long high_pulse = speed_to_high_pulse(umd, speed);
	if(!umd || (high_pulse < 0)){
		print_err("%s: invalid parameter\n", __func__);
		return -1;
	}
	umd->speed = angle;
	pwm_set_high_pulse(umd->tim, umd->channel, high_pulse);
	return 0;
}
void us_motor_upper_edge_reached(struct us_motor_device *umd)
{
    return 0;
}
void us_motor_middle_point_reached(struct us_motor_device *umd)
{
    return 0;
}
void us_motor_lower_edge_reached(struct us_motor_device *umd)
{
    return 0;
}
