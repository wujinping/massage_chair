
#pragma once
#include "platform.h"


enum motor_type {
    MOTOR_FOR_BACK = 0,
    MOTOR_FOR_LEG = 1,
};
enum motor_speed {
    SPEED_STATIC,
    SPEED_SLOW,
    SPEED_MEDIUM,
    SPEED_FAST
};
enum motion_direction {
    DIR_FORWARD,
    DIR_BACKWARD
};
enum motion_range {
    RANGE_UPPER_HALF,
    RANGE_LOWER_HALF,
    RANGE_ENTIRE,
};
struct us_motor_device {
	enum motor_type type;	
	TIM_TypeDef *tim;
	struct gpio pwm;
	struct gpio dir;
	struct gpio upper_edge;
	struct gpio middle_pos;
	struct gpio lower_edge;
	uint8_t channel;
	enum motor_speed speed;	
	void (*upper_edge_reached)(struct us_motor_device *umd);
	void (*middle_point_reached)(struct us_motor_device *umd);
	void (*lower_edge_reached)(struct us_motor_device *umd);
};
struct us_motor_init_para{
	enum motor_type type;
	TIM_TypeDef *tim;
	uint8_t channel;
	struct gpio pwm;
	struct gpio dir;
	struct gpio upper_edge;
	struct gpio middle_pos;
	struct gpio lower_edge;
	enum motor_speed default_speed;	
	void (*upper_edge_reached)(struct us_motor_device *umd);
	void (*middle_point_reached)(struct us_motor_device *umd);
	void (*lower_edge_reached)(struct us_motor_device *umd);
};
int us_motor_init(struct us_motor_device **pum, struct us_motor_init_para *para);
int us_motor_set_speed(struct us_motor_device *umd, enum motor_speed speed);
int us_motor_set_dir(struct us_motor_device *umd, enum motion_direction dir);
int us_motor_set_range(struct us_motor_device *umd, enum motion_range range);
