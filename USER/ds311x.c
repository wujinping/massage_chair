#include "ds311x.h"

long angle_to_high_pulse(struct ds311x_device *ds311x, uint16_t angle)
{
	long high_pulse = 0;	
	if(angle < TURNING_ANGLE_MIN || angle > ds311x->max_angle){
		return -1;
	}
	/* hihg pulse range from 0.5ms to 2.5ms to define a range of 0 degree to 270 degrees */
	high_pulse = 500 +  ((2500 - 500)*angle)/ds311x->max_angle;
	return high_pulse;
}
long high_pulse_to_angle(uint16_t high_pulse)
{
	if(high_pulse < 0 || high_pulse > 20000){
		return -1;
	}
	
}
int ds311x_init(struct ds311x_device **pds311x, struct ds311x_init_para *para)
{
	struct ds311x_device *ds311x = calloc(sizeof(struct ds311x_device), 1);	
	if(!pds311x || !para)
		return -1;
	if(!ds311x){
		print_err("%s: not enough memory\n", __func__);
		return -1;
	}
	ds311x->pwm = para->pwm;
	ds311x->angle = para->default_angle;
	ds311x->tim = para->tim;
	ds311x->channel = para->channel;
	ds311x->type = para->type;

	switch((uint8_t)ds311x->type){
		case DS3115:
			ds311x->max_angle = DS3115_ANGLE_MAX;
			break;
		case DS3118:
			ds311x->max_angle = DS3118_ANGLE_MAX;			
			break;		
	}
	*pds311x = ds311x;	
	pwm_init(ds311x->tim, ds311x->channel, &ds311x->pwm, para->default_angle);
	ds311x_set_angle(ds311x, para->default_angle);
	delay_ms(100);
	
	return 0;
}

int ds311x_set_angle(struct ds311x_device *ds311x, uint16_t angle)
{
	long high_pulse = angle_to_high_pulse(ds311x, angle);
	if(!ds311x || (high_pulse < 0)){
		print_err("%s: invalid parameter\n", __func__);
		return -1;
	}
	ds311x->angle = angle;
	pwm_set_high_pulse(ds311x->tim, ds311x->channel, high_pulse);
	return 0;
}
int ds311x_set_angle_delayed(uint16_t angle, uint16_t delay, struct ds311x_device *ds311x)
{
	long high_pulse = angle_to_high_pulse(ds311x, angle);
	if(!ds311x || (high_pulse < 0)){
		print_err("%s: invalid parameter\n", __func__);
		return -1;
	}
	ds311x->angle = angle;
	pwm_set_high_pulse(ds311x->tim, ds311x->channel, high_pulse);
	if(delay)
		delay_ms(delay);
	return 0;
}
int ds311x_set_angle_relative(struct ds311x_device *ds311x, int16_t relative_angle)
{
	long high_pulse = 0;
	int16_t angle = ds311x->angle + relative_angle;
	if(angle < 0 || angle > ds311x->max_angle){
		print_err("%s: invalid parameter\n", __func__);
		return -1;		
	}
	high_pulse = angle_to_high_pulse(ds311x, angle);
	if(!ds311x || (high_pulse < 0)){
		print_err("%s: invalid parameter\n", __func__);
		return -1;
	}
	ds311x->angle = angle;
	pwm_set_high_pulse(ds311x->tim, ds311x->channel, high_pulse);
	return 0;
}		
int ds311x_set_angle_relative_delayed(int16_t relative_angle, uint16_t delay, struct ds311x_device *ds311x)
{
	long high_pulse = 0;
	int16_t angle = ds311x->angle + relative_angle;
	if(angle < 0 || angle > ds311x->max_angle){
		print_err("%s: invalid parameter\n", __func__);
		return -1;		
	}
	high_pulse = angle_to_high_pulse(ds311x, angle);
	if(!ds311x || (high_pulse < 0)){
		print_err("%s: invalid parameter\n", __func__);
		return -1;
	}
	ds311x->angle = angle;
	pwm_set_high_pulse(ds311x->tim, ds311x->channel, high_pulse);
	if(delay)
		delay_ms(delay);
	return 0;
}		