#include "platform.h"
#include "ble102.h"

const char *uuid_tx = "00001801-0000-1000-8000-00805f9b34fb";
const char *uuid_rx = "00001800-0000-1000-8000-00805f9b34fb";
const char *uuid_serial = "0003cdd0-0000-1000-8000-00805f9b0131";

extern struct controller *ctrler;
int ble10x_device_init(struct ble10x_device **pdev, struct ble10x_init_para *para)
{
    int ret = -1;
    struct ble10x_device *dev = calloc(sizeof(struct ble10x_device), 1);	
    if(!pdev || !para)
	return -1;
    if(!dev){
	print_err("%s: not enough memory\n", __func__);
	return -1;
    }
    dev->usart = para->usart;
    dev->reset = para->reset;
    dev->reload = para->reload;
    dev->wakeup = para->wakeup;
    dev->usart_baudrate = para->default_baudrate;
    *pdev = dev;
    /* TODO: Initialize the GPIOs of module, because of a lack of time, we just ignore them */
    init_module_gpios(dev);
    /* Initialize the USART port */
    setup_serial_port(dev, para);
    /* TODO: Initialize transmit and recv buffer */
    setup_buffers(dev);
    if(para->init_everytime){
	/* Now we try to enter the command mode see if the module works */
	ret = enter_command_mode(dev);
	if(ret < 0){
	    dev_err("%s: failed to enter command mode.\n", __func__);
	    return -1;
	}
	ret = set_workmode(dev, MODE_SLAVE);
	if(ret < 0){
	    dev_err("%s: failed to set work mode.\n", __func__);
	    return -1;
	}
	ret = set_uuid(dev);
	if(ret < 0){
	    dev_err("%s: failed to set work mode.\n", __func__);
	    return -1;
	}
    }
    dev->msg_x = ble10x_xmit_msg;

    return 0;
}
int setup_serial_port(struct ble10x_device *dev, struct ble10x_init_para *para)
{
    if(!dev){
	print_err("%s: Invalid parameter\n", __func__);
	return -1;
    }
    dev->usx = calloc(sizeof(struct usart_xmitter), 1);
    if(!dev->usx){
	print_err("%s: Not enough memory\n", __func__);
	return -1;
    }
    dev->usx->xmit_cb = ble10x_usart_xmit;
    dev->usx->recv_cb = ble10x_usart_recv;
		dev->usx->usart_baudrate = para->default_baudrate;
    /* TODO: First initialize the usart port with given parameters */
    /* FIXME:!!No enough time, here we do a hard initialization(take no given para into consideration) */	
    uart3_init(dev->usx->usart_baudrate);
    /* TODO: add timmer initialization here */
    platform_timer_intr_init(10);
}
int setup_buffers(struct ble10x_device *dev)
{
    if(!dev){
	print_err("%s: Invalid parameter\n", __func__);
	return -1;
    }
    dev->xmit_buf = calloc(XMIT_BUFFER_MAX, 1);
    if(!dev->xmit_buf){
	dev_err("%s: failed to allocate buffer for transmission.\n", __func__);
	return -1;
    }
    dev->recv_buf = calloc(RECV_BUFFER_MAX, 1);
    if(!dev->recv_buf){
	dev_err("%s: failed to allocate buffer for reception.\n", __func__);
	return -1;
    }
    dev_info("%s: buffer allocation succeed.\n", __func__);
    return 0;
}
int ble10x_usart_recv(struct ble10x_device *dev, char *buf, uint16_t len)
{
    if(!dev || !buf || len <= 5){
	print_err("%s: Invalid parameter\n", __func__);
	return -1;
    }
    /* First copy the received data into our internal buffer */
    memcpy(dev->recv_buf, buf, len);
    /* Now we try to decode the received string, if it's the AT command response, handle it here */
    if(!strncmp(dev->recv_buf, "\r\n+", 3)){
    }
    else{
	dev->msg_cb(ctrler, dev->recv_buf, len);
    }
    return 0;
}
int ble10x_usart_xmit(struct ble10x_device *dev, char *buf, uint16_t len)
{
    return 0;
}
int ble10x_set_recv_callback(struct ble10x_device *dev, msg_callback callback)
{
    if(!dev){
	print_err("%s: Invalid parameter\n", __func__);
	return -1;
    }
    dev->msg_cb = callback;
    return 0;

}
int ble10x_xmit_msg(struct ble10x_device *dev, char *msg, uint16_t msg_len)
{
    return 0;
}
static int set_workmode(struct ble10x_device *dev, enum MODULE_WORK_MODE mode)
{
    return 0;
}
static int set_uuid(struct ble10x_device *dev)
{
    return 0;
}
static int set_name(struct ble10x_device *dev, char *name)
{
    return 0;
}
static int set_uart_packtime(struct ble10x_device *dev, uint16_t pack_time)
{
    return 0;
}
static int exit_command_mode(struct ble10x_device *dev)
{
    return 0;
}
static int enter_command_mode(struct ble10x_device *dev)
{
    return 0;
}
static int init_module_gpios(struct ble10x_device *dev)
{
    return 0;
}
