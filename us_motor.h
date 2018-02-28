
#pragma once
#include "platform.h"


enum motor_type {
    MOTOR_FOR_BACK = 0,
    MOTOR_FOR_DOWN = 1,
};
enum speed_rank {
    SPEED_STATIC,
    SPEED_SLOW,
    SPEED_MEDIUM,
    SPEED_FAST
};
enum motion_direction {
    DIR_UPWARDS,
    DIR_DOWNWARDS
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
	struct gpio upper_edge_reached;
	struct gpio middle_pos_reached;
	struct gpio lower_edge_reached;
	uint8_t channel;
	enum speed_rank speed;	
};
struct us_motor_init_para{
	enum motor_type type;
	TIM_TypeDef *tim;
	uint8_t channel;
	struct gpio pwm;
	struct gpio dir;
	struct gpio upper_edge_reached;
	struct gpio middle_pos_reached;
	struct gpio lower_edge_reached;
	enum speed_rank default_speed;	
};
int us_motor_init(struct us_motor_device **pum, struct us_motor_init_para *para);
int us_motor_set_speed(struct us_motor_device *umd, enum speed_rank speed);
int us_motor_set_dir(struct us_motor_device *umd, enum motion_direction dir);
int us_motor_set_range(struct us_motor_device *umd, enum motion_range range);
