#pragma once

#include "platform.h"
#define nrf_ce_low(p)       do{GPIO_ResetBits(p->ce.port, p->ce.pin);}while(0)
#define nrf_ce_high(p)      do{GPIO_SetBits(p->ce.port, p->ce.pin);}while(0)

#define spi_nss_low(p) 	   do{GPIO_ResetBits(p->nss.port, p->nss.pin);}while(0)
#define spi_nss_high(p) 	   do{GPIO_SetBits(p->nss.port, p->nss.pin);}while(0)

//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  32  	// 20 uints TX payload
#define RX_PLOAD_WIDTH  32  	// 20 uints TX payload

#define ADDR_MAX_NUM	255

#define MAX_TX  		0x10  //达到最大发送次数中断
#define TX_OK   		0x20  //TX发送完成中断
#define RX_OK   		0x40  //接收到数据中断
//***************************************NRF24L01寄存器指令*******************************************************
#define RF_READ_REG        0x00  	// 读寄存器指令
#define RF_WRITE_REG       0x20 	// 写寄存器指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xA0  	// 写待发数据指令
#define FLUSH_TX        0xE1 	// 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define NOP             0xFF  	// 保留
//*************************************SPI(nRF24L01)寄存器地址****************************************************
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define STATUS          0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道0接收数据长度
#define RX_PW_P2        0x13  // 接收频道0接收数据长度
#define RX_PW_P3        0x14  // 接收频道0接收数据长度
#define RX_PW_P4        0x15  // 接收频道0接收数据长度
#define RX_PW_P5        0x16  // 接收频道0接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置
enum work_mode{
	MODE_TX,
	MODE_RX
};
struct nrf24xx_init_para {
	SPI_TypeDef *spi_base;
	struct gpio sclk;
	struct gpio mosi;
	struct gpio miso;
	struct gpio ce;
	struct gpio nss;	
	struct gpio irq;
	enum work_mode mode;
	uint8_t payload_len;
	uint8_t module_index;
};
struct nrf24xx {
	struct spi_dev *spi;
	struct gpio irq;
	enum work_mode mode;	
	uint8_t payload_len;	
	uint8_t module_index;	
};

void nrf24xx_tx_packet(struct nrf24xx *nrf, uint8_t index, uint8_t * tx_buf);
int nrf24xx_irq_handler(struct nrf24xx *nrf);
int nrf24xx_init(struct nrf24xx **pnrf, struct nrf24xx_init_para *para);
void nrf_set_rx(struct nrf24xx *nrf);
void nrf_set_tx(struct nrf24xx *nrf);



