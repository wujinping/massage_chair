#pragma once
#include "platform.h"
/* IMPORTANT: maxium resolution of turning angle is 0.2 degree, so we use TURNING_ANGLE * 10 to define the actual turning angle */
#define TURNING_ANGLE_MIN		0
#define DS3118_ANGLE_MAX		270
#define DS3115_ANGLE_MAX		180

enum ds311x_type {
	DS3115 = 0,
	DS3118
};
struct ds311x_device {
	enum ds311x_type type;	
	TIM_TypeDef *tim;
	struct gpio pwm;
	uint8_t channel;
	uint16_t max_angle;
	uint16_t angle;	
};
struct ds311x_init_para{
	enum ds311x_type type;
	TIM_TypeDef *tim;
	uint8_t channel;
	struct gpio pwm;
	uint16_t default_angle;
};
int ds311x_init(struct ds311x_device **pds311x, struct ds311x_init_para *para);
int ds311x_set_angle(struct ds311x_device *ds311x, uint16_t angle);
int ds311x_set_angle_delayed(uint16_t angle, uint16_t delay, struct ds311x_device *ds311x);
int ds311x_set_angle_relative(struct ds311x_device *ds311x, int16_t relative_angle);
int ds311x_set_angle_relative_delayed(int16_t relative_angle, uint16_t delay, struct ds311x_device *ds311x);
