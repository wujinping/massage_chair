#pragma once
#include "platform.h"
#include "usart.h"

#define PACKET_SIZE_MAX	20
#define XMIT_BUFFER_MAX	PACKET_SIZE_MAX
#define RECV_BUFFER_MAX	PACKET_SIZE_MAX
typedef int (*msg_callback)(void *, char *, uint16_t);
typedef int (*msg_xmit)(char *, uint16_t, void *);

enum MODULE_WORK_MODE {
    MODE_INVALID,
    MODE_COMMAND,
    MODE_MASTER,
    MODE_SLAVE,
    MODE_TRANSPARENT,
    MODE_IBEACON
};
struct usart_xmitter {
	USART_TypeDef *usart;
	int usart_baudrate;
	int (*recv_cb)(void *, char *, uint16_t);
	int (*xmit_cb)(void *, char *, uint16_t);
};
struct ble10x_device {
	USART_TypeDef *usart;
	struct gpio reset;
	struct gpio reload;
	struct gpio wakeup;
	int usart_baudrate;
	enum MODULE_WORK_MODE work_mode;
	struct usart_xmitter *usx;
	char *xmit_buf;
	char *recv_buf;
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
static int module_reset(struct ble10x_device *dev);

int ble10x_usart_recv(struct ble10x_device *dev, char *buf, uint16_t len);

int ble10x_usart_xmit(struct ble10x_device *dev, char *buf, uint16_t len);

int setup_serial_port(struct ble10x_device *dev, struct ble10x_init_para *para);
/* Setup ble102 usart transmission and reception buffers */
static int setup_buffers(struct ble10x_device *dev);

int ble10x_xmit_msg(char *msg, uint16_t msg_len, struct ble10x_device *dev);


int ble10x_device_init(struct ble10x_device **pdev, struct ble10x_init_para *para);
int ble10x_set_recv_callback(struct ble10x_device *dev, msg_callback callback);
