/***************************************************************************//**
 *   @file   platform.h
 *   @brief  platform specific defines and function headers, should be 
 *   fullfilled according to the application platform .
*******************************************************************************/
#pragma once
#include "def.h"
#include "stm32f10x.h"
#include "spi.h"


#if	STM32F10X_HD

struct gpio{
	GPIO_TypeDef *port;
	uint16_t	pin;
	GPIOMode_TypeDef mode;
};

struct spi_dev {
	SPI_TypeDef *spi_base;
	struct gpio sclk;
	struct gpio mosi;
	struct gpio miso;
	struct gpio ce;
	struct gpio nss;
};
#else
struct gpio{
};
#endif

struct platform_io_conf{
	struct gpio adf7023_cs;
	struct gpio adf7023_miso;		
	struct gpio adf7023_irq;		
};

void gpio_init(struct gpio *pio);

void gpio_set_value(struct gpio *pio, u8 value);

u8 gpio_get_value(struct gpio *pio);

void gpio_set_direction(struct gpio *pio, u8 dir);

void adf7023_cs_assert(struct spi_dev *spi);

uint8_t adf7023_miso_value(struct spi_dev *spi);

void adf7023_cs_deassert(struct spi_dev *spi);


char plat_gpio_init(void);

void plat_spi_write_read(struct spi_dev *spi, uint8_t *pdata, uint8_t unused);

void platform_spi_init(struct spi_dev *spi, uint32_t freq);

char plat_intr_init(struct gpio *pio);

void platform_init_dist_timer(void);

void pwm_init(TIM_TypeDef *tim, uint8_t channel, struct gpio *pio, uint32_t high_pulse);
	
void pwm_set_high_pulse(TIM_TypeDef *tim, uint8_t channel, uint32_t high_pulse);