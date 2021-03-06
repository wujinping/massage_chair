#include "spi.h"
#include "platform.h"
  
//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：

void spi_set_speed(SPI_TypeDef *spi_base, u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	spi_base->CR1&=0XFFC7;
	spi_base->CR1|=SPI_BaudRatePrescaler;
	SPI_Cmd(spi_base,ENABLE); 
} 
void spi_write_read(SPI_TypeDef *spi_base, u8 *pdata)
{		 			 
	uint8_t value;

	while (SPI_I2S_GetFlagStatus(spi_base, SPI_I2S_FLAG_TXE) == RESET){}

	SPI_I2S_SendData(spi_base, *pdata); 

	while (SPI_I2S_GetFlagStatus(spi_base, SPI_I2S_FLAG_RXNE) == RESET){}

	value = SPI_I2S_ReceiveData(spi_base);
	*pdata = value;

}

void spi_init(struct spi_dev *spi, uint32_t freq)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	uint8_t prescaler, value = 0xff;
	if(!spi){
		dev_err("%s: invalid parameter, spi handle = NULL\n", __func__);
		return;
	}
	switch((uint32_t)spi->spi_base){
		case (uint32_t)SPI1:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
			break;
		case (uint32_t)SPI2:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
			break;
		case (uint32_t)SPI3:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
			break;
		default:
			break;
	}

	RCC_APB2PeriphClockCmd(1 << ((spi->miso.port - GPIOA)/(GPIOB - GPIOA) + 2),ENABLE); 
	RCC_APB2PeriphClockCmd(1 << ((spi->mosi.port - GPIOA)/(GPIOB - GPIOA) + 2),ENABLE); 
	RCC_APB2PeriphClockCmd(1 << ((spi->sclk.port - GPIOA)/(GPIOB - GPIOA) + 2),ENABLE); 
	/* configure gpio */
	GPIO_InitStructure.GPIO_Pin = spi->miso.pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(spi->miso.port, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = spi->mosi.pin;
	GPIO_Init(spi->mosi.port, &GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin = spi->sclk.pin;
	GPIO_Init(spi->sclk.port, &GPIO_InitStructure); 

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(spi->spi_base, &SPI_InitStructure); 


	SPI_Cmd(spi->spi_base, ENABLE); 

	spi_set_speed(spi->spi_base, SPI_BaudRatePrescaler_4);
	spi_write_read(spi->spi_base, &value);

} 






