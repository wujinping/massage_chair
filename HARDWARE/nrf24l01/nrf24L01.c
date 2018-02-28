#include "stm32f10x.h"
#include "nrf24xx.h"
#include "transmitter.h"


uint8_t  TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//本地地址
uint8_t  RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//接收地址

const uint8_t addr_default[5] = {0x00,0x43,0x10,0x10,0x05};

//uint8_t addr_index[11][5] = {
//	{0x00,0x43,0x10,0x10,0x05},
//	{0x01,0x43,0x10,0x10,0x05},
//	{0x02,0x43,0x10,0x10,0x05},
//	{0x03,0x43,0x10,0x10,0x05},
//	{0x04,0x43,0x10,0x10,0x05},
//	{0x05,0x43,0x10,0x10,0x05},
//	{0x06,0x43,0x10,0x10,0x05},
//	{0x07,0x43,0x10,0x10,0x05},
//	{0x08,0x43,0x10,0x10,0x05},
//	{0x09,0x43,0x10,0x10,0x05},
//	{0x0a,0x43,0x10,0x10,0x05},	
//};
uint8_t nrf_write_reg(struct spi_dev *spi, uint8_t reg, uint8_t value);
static uint8_t spi_write_buf(struct spi_dev *spi,uint8_t reg, uint8_t *pBuf, uint8_t uchars);
static uint8_t spi_read_buf(struct spi_dev *spi,uint8_t reg, uint8_t *pBuf, uint8_t uchars);
extern void delay_us(uint16_t t);


uint8_t SPI_ReadWriteByte(struct spi_dev *spi, uint8_t data)		
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(spi->spi_base, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(spi->spi_base, data);
    
  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(spi->spi_base, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(spi->spi_base);                                              
}

/*
*****************************************************************
* 初始化
*****************************************************************
*/
int init_nrf24l01(struct nrf24xx *nrf)
{
	int ret = 0;
	u8 temp_buf[5]={0};
	uint8_t real_addr[5];
	memcpy(real_addr, addr_default, 5);
	real_addr[0] = nrf->module_index;
	
	delay_us(100);
	nrf_ce_low(nrf->spi);
  spi_nss_high(nrf->spi);
	spi_write_buf(nrf->spi, RF_WRITE_REG + TX_ADDR, real_addr, TX_ADR_WIDTH);    
	spi_write_buf(nrf->spi, RF_WRITE_REG + RX_ADDR_P0, real_addr, RX_ADR_WIDTH); 
	nrf_write_reg(nrf->spi, RF_WRITE_REG + EN_AA, 0x01);      
	nrf_write_reg(nrf->spi, RF_WRITE_REG + EN_RXADDR, 0x01); 
	spi_read_buf(nrf->spi, TX_ADDR, temp_buf, TX_ADR_WIDTH);
	if((temp_buf[0]!= real_addr[0]) || (temp_buf[1]!= real_addr[1])){
		print_err("%s: nrf24l01 device initialize failed\n", __func__);
		ret = -1;
	}			
	nrf_write_reg(nrf->spi, RF_WRITE_REG + SETUP_RETR, 0x1a); 
	nrf_write_reg(nrf->spi, RF_WRITE_REG + RF_CH, 0);        
	nrf_write_reg(nrf->spi, RF_WRITE_REG + RX_PW_P0, nrf->payload_len); 
	nrf_write_reg(nrf->spi, RF_WRITE_REG + RF_SETUP, 0x0f);  
	delay_us(100);
	if(MODE_RX == nrf->mode){
		nrf_set_rx(nrf);
	}
	else{
		nrf_set_tx(nrf);
	}
	return ret;
}
int nrf24xx_init(struct nrf24xx **pnrf, struct nrf24xx_init_para *para)
{
	int ret = 0;
	struct nrf24xx *nrf = calloc(sizeof(struct nrf24xx), 1);	
	if(!pnrf){
		return -1;
	}
	if(!nrf){
		return -1;
	}
	nrf->spi = calloc(sizeof(struct spi_dev), 1);
	if(!nrf->spi){
		return -1;
	}
	nrf->spi->spi_base = para->spi_base;
	nrf->spi->ce = para->ce;
	nrf->spi->miso = para->miso;
	nrf->spi->mosi = para->mosi;
	nrf->spi->nss = para->nss;
	nrf->spi->sclk = para->sclk;
	nrf->irq = para->irq;
	nrf->mode = para->mode;
	nrf->payload_len = para->payload_len;
	nrf->module_index = para->module_index;
	platform_spi_init(nrf->spi, 1000000);
	ret = init_nrf24l01(nrf);
	if(ret < 0){
		return -1;
	}
	*pnrf = nrf;
	plat_intr_init(&nrf->irq);
	return 0;
}

uint8_t nrf_write_reg(struct spi_dev *spi, uint8_t reg, uint8_t value)
{
	uint8_t status;
	spi_nss_low(spi);					 
	status = SPI_ReadWriteByte(spi,reg); 
	SPI_ReadWriteByte(spi,value);	
	spi_nss_high(spi);				
    return 	status;
}

static uint8_t nrf_read_reg(struct spi_dev *spi, uint8_t reg)
{
	uint8_t reg_val;
	spi_nss_low(spi);					
	SPI_ReadWriteByte(spi,reg);		
	reg_val = SPI_ReadWriteByte(spi,0);	
	spi_nss_high(spi);				
    return 	reg_val;
}

static uint8_t spi_write_buf(struct spi_dev *spi, uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	spi_nss_low(spi);				     
	status = SPI_ReadWriteByte(spi, reg);
	for(i=0; i<uchars; i++)
	{
		SPI_ReadWriteByte(spi, pBuf[i]);	
	}
	spi_nss_high(spi);					
    return 	status;	
}

static uint8_t spi_read_buf(struct spi_dev *spi, uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	spi_nss_low(spi);						
	status = SPI_ReadWriteByte(spi,reg);	
	for(i=0; i<uchars; i++)
	{
		pBuf[i] = SPI_ReadWriteByte(spi,0); 
	}
	spi_nss_high(spi);					
    return 	status;
}



void nrf_set_rx(struct nrf24xx *nrf)
{

	nrf_ce_low(nrf->spi);
  nrf_write_reg(nrf->spi, RF_WRITE_REG+CONFIG, 0x0f);
	nrf_ce_high(nrf->spi);

	delay_us(130);
} 

void nrf_set_tx(struct nrf24xx *nrf)
{
	nrf_ce_low(nrf->spi);

  spi_nss_high(nrf->spi);
  nrf_write_reg(nrf->spi, RF_WRITE_REG+CONFIG,0x0e); 
	nrf_write_reg(nrf->spi, FLUSH_TX,0xff);
	nrf_write_reg(nrf->spi,RF_WRITE_REG+STATUS,0x40);   
	nrf_ce_high(nrf->spi);
	delay_us(500);
} 

uint8_t nrf24xx_rx_packet(struct nrf24xx *nrf, uint8_t * rx_buf)
{
		nrf_ce_low(nrf->spi);
		spi_read_buf(nrf->spi,RD_RX_PLOAD,rx_buf,nrf->payload_len);// read receive payload from RX_FIFO buffer
		nrf_write_reg(nrf->spi, FLUSH_RX,0xff);
		nrf_ce_high(nrf->spi);
}

void nrf24xx_tx_packet(struct nrf24xx *nrf, uint8_t index, uint8_t *tx_buf)
{	
	uint8_t sta=0;
	uint8_t real_addr[5];
	memcpy(real_addr, addr_default, 5);
	real_addr[0] = index;
	nrf_ce_low(nrf->spi);		
	spi_write_buf(nrf->spi, RF_WRITE_REG + TX_ADDR, real_addr, TX_ADR_WIDTH); 
	spi_write_buf(nrf->spi, RF_WRITE_REG + RX_ADDR_P0, real_addr, RX_ADR_WIDTH); 	
	spi_write_buf(nrf->spi, WR_TX_PLOAD, tx_buf, nrf->payload_len); 		
	delay_us(10);
	nrf_ce_high(nrf->spi);	
	delay_us(10);
	nrf_ce_low(nrf->spi);		 

}

int nrf24xx_irq_handler(struct nrf24xx *nrf)
{
	uint8_t *rx_buf = NULL;
	uint8_t status;
	status = nrf_read_reg(nrf->spi,STATUS);	
	nrf_write_reg(nrf->spi, RF_WRITE_REG+STATUS,status);
	if(MODE_TX == nrf->mode){
		if(status&MAX_TX)	{
			nrf_write_reg(nrf->spi, FLUSH_TX,0xff);
			return -1;
		}	
		else{
			return 0;
		}
	}
	else{
		if(status & RX_OK){
			rx_buf = calloc(nrf->payload_len, 1);
			nrf24xx_rx_packet(nrf, rx_buf);
			transmitter_handle_downstream(rx_buf, nrf->payload_len);
		}
	}
	return 0;
}




