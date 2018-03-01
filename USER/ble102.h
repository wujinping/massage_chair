#pragma once
#include "platform.h"
typedef (*msg_callback)(void *, char *, uint16_t);
typedef (*msg_xmit)(void *, char *, uint16_t);
enum MODULE_WORK_MODE {
    MODE_INVALID,
    MODE_COMMAND,
    MODE_MASTER,
    MODE_SLAVE,
    MODE_TRANSPARENT,
    MODE_IBEACON
};
struct ble10x_device {
	USART_TypeDef *usart;
	struct gpio reset;
	struct gpio reload;
	struct gpio wakeup;
	int usart_baudrate;
	enum MODULE_WORK_MODE work_mode;
	msg_callback msg_cb;
	msg_xmit msg_x;
};
struct ble10x_init_para {
	USART_TypeDef *usart;
	struct gpio reset;
	struct gpio reload;
	struct gpio wakeup;
	int default_baudrate;
	enum MODULE_WORK_MODE default_mode;
	int init_everytime;
};
static int set_workmode(struct ble10x_device *dev, enum MODULE_WORK_MODE);
/* Not implemented yet ~ */
static int set_uuid(struct ble10x_device *dev);
/* Not implemented yet ~ */
static int set_name(struct ble10x_device *dev, char *name);
/* Not implemented yet ~ */
static int set_uart_packtime(struct ble10x_device *dev, uint16_t pack_time);
/* Needs to be implemented */
static int exit_command_mode(struct ble10x_device *dev);
/* Needs to be implemented */
static int enter_command_mode(struct ble10x_device *dev);
/* Not implemented yet ~ */
static int init_module_gpios(struct ble10x_device *dev);

int ble10x_xmit_msg(struct ble10x_device *dev, char *msg, uint16_t msg_len);


int ble10x_device_init(struct ble10x_device **pdev, struct ble10x_init_para *para);
int ble10x_set_recv_callback(struct ble10x_device *dev, msg_callback callback);