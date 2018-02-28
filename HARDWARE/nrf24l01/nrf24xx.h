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

#define MAX_TX  		0x10  //�ﵽ����ʹ����ж�
#define TX_OK   		0x20  //TX��������ж�
#define RX_OK   		0x40  //���յ������ж�
//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define RF_READ_REG        0x00  	// ���Ĵ���ָ��
#define RF_WRITE_REG       0x20 	// д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP             0xFF  	// ����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
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



