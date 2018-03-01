#pragma once
#include "def.h"
#include "us_motor.h"
#include "stm32f10x.h"
#include "platform.h"

struct controller {
    struct gpio upper_start_range;
    struct gpio lower_start;
    struct gpio speed_config;
    struct us_motor_device *bdev;
    struct us_motor_device *udev;
};
struct controller_init_para {
    struct gpio upper_start_range;
    struct gpio lower_start;
    struct gpio speed_config;
    struct us_motor_init_para *bpara;
    struct us_motor_init_para *upara;
};
void xmit_list_push(u8 *packet, u16 packet_len);
struct xmit_list * xmit_list_pop(void);

int controller_init(struct controller **pctrler, struct controller_init_para *para);
