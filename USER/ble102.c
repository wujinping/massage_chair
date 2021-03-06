#include "platform.h"
#include "ble102.h"
#include "delay.h"

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
    dev_err("%s: not enough memory\n", __func__);
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
  dev->work_mode = para->default_mode;
  if(para->init_everytime){
    /* Now we try to enter the command mode see if the module works */
    ret = module_reset(dev);
    if(ret < 0){
      dev_err("%s: failed to reset ble10x module.\n", __func__);
      return -1;
    }
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
    ret = exit_command_mode(dev);
    if(ret < 0){
      dev_err("%s: failed to exit command mode.\n", __func__);
      return -1;
    }
  }
  dev->msg_x = ble10x_xmit_msg;

  return 0;
}
int setup_serial_port(struct ble10x_device *dev, struct ble10x_init_para *para)
{
  if(!dev){
    dev_err("%s: Invalid parameter\n", __func__);
    return -1;
  }
  dev->usx = calloc(sizeof(struct usart_xmitter), 1);
  if(!dev->usx){
    dev_err("%s: Not enough memory\n", __func__);
    return -1;
  }
  dev->usx->xmit_cb = ble10x_usart_xmit;
  dev->usx->recv_cb = ble10x_usart_recv;
  dev->usx->usart_baudrate = para->default_baudrate;
  /* FIXME:!!No enough time, here we do a hard initialization(take no given para into consideration) */	
  uart3_init(dev->usx->usart_baudrate);
  /* The timer is used for SERIAL data package, an interrupt will be generated by the timer
   *   when more than 100ms is passed without any data from serial port
   */
  platform_timer_intr_init(100);
}
int setup_buffers(struct ble10x_device *dev)
{
  if(!dev){
    dev_err("%s: Invalid parameter\n", __func__);
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
    //dev_err("%s: Invalid parameter\n", __func__);
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
    dev_err("%s: Invalid parameter\n", __func__);
    return -1;
  }
  dev->msg_cb = callback;
  return 0;

}
int ble10x_xmit_msg(char *msg, uint16_t msg_len, struct ble10x_device *dev)
{
  if(!dev || !msg){
    dev_err("%s: Invalid parameter\n", __func__);
    return -1;
  }
  serial_xfer_string(msg, msg_len);
  /* FIXME: free a pointer here is dangerous, PLEASE be sure the 'msg' variable is a pointer allocated from heap */
  free(msg);
  return 0;
}
static int set_workmode(struct ble10x_device *dev, enum MODULE_WORK_MODE mode)
{
  char *str = "AT+MODE=S\r\n";
  serial_xfer_string(str, 11);
  delay_ms(20);
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
  char *str = "AT+ENTM\r\n";
  serial_xfer_string(str, 9);
  delay_ms(20);
  return 0;
}
static int enter_command_mode(struct ble10x_device *dev)
{
  char *str = "+++a";
  serial_xfer_string(str, 4);
  delay_ms(20);
  return 0;
}
static int module_reset(struct ble10x_device *dev)
{
  gpio_set_value(&dev->reset, 0);	
  delay_ms(200);
  gpio_set_value(&dev->reset, 1);	
}
static int init_module_gpios(struct ble10x_device *dev)
{
  gpio_init(&dev->reload);
  gpio_init(&dev->wakeup);
  gpio_init(&dev->reset);
  gpio_set_value(&dev->reload, 1);
  gpio_set_value(&dev->wakeup, 1);
  gpio_set_value(&dev->reset, 0);	
  delay_ms(200);
  gpio_set_value(&dev->reset, 1);	
  return 0;
}
