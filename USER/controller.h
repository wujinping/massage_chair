#pragma once
#include "def.h"
#include "us_motor.h"
#include "stm32f10x.h"
#include "platform.h"
#include "ble102.h"
enum upper_state {
	STATE_STATIC,
	STATE_UPPER,
	STATE_LOWER,
	STATE_ENTIRE
};
enum lower_state {
	STATE_OFF,
	STATE_ON
};
struct controller {
    int unlocked;
    uint32_t valid_sec;
    uint32_t valid_start;
    struct gpio upper_start_range;
    struct gpio lower_start;
    struct gpio speed_config;
    struct gpio beeper;
	struct gpio speed_led;
	struct gpio upper_led1;
	struct gpio upper_led2;
	struct gpio lower_led;
    struct us_motor_device *bdev;
    struct us_motor_device *udev;
	  struct ble10x_device *ble_dev;
	enum upper_state ustate;
	enum lower_state lstate;
	enum motor_speed speed;
  uint32_t id_buf[3];
		int (*back_key_pressed)(struct controller *ctrler);
	int (*bottom_key_pressed)(struct controller *ctrler);
	int (*speed_key_pressed)(struct controller *ctrler);
};
struct controller_init_para {
    struct gpio upper_start_range;
    struct gpio lower_start;
    struct gpio speed_config;
    struct gpio beeper;
	struct gpio speed_led;
	struct gpio upper_led1;
	struct gpio upper_led2;
	struct gpio lower_led;
    struct us_motor_init_para *bpara;
    struct us_motor_init_para *upara;
	  struct ble10x_init_para *ble10x_init_para;
};
void xmit_list_push(u8 *packet, u16 packet_len);
struct xmit_list * xmit_list_pop(void);
int beeper_work(uint8_t count, uint8_t unused, struct controller *ctrler);

int controller_init(struct controller **pctrler, struct controller_init_para *para);
int controller_back_btn_pressed(struct controller *ctrler);
int controller_bottom_btn_pressed(struct controller *ctrler);
int controller_speed_btn_pressed(struct controller *ctrler);
int controller_usage_verify(struct controller *ctrler);
