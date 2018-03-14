/***************************************************************************//**
 *   @file   platform.h
 *   @brief  platform specific defines and function headers, should be 
 *   fullfilled according to the application platform .
*******************************************************************************/
#pragma once
#include "def.h"
#include "stm32f10x.h"
#include "spi.h"


#if	1

#define COUNTER_INTERVAL 100

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

char plat_gpio_init(void);

void plat_spi_write_read(struct spi_dev *spi, uint8_t *pdata, uint8_t unused);

void platform_spi_init(struct spi_dev *spi, uint32_t freq);

char plat_intr_init(struct gpio *pio, EXTITrigger_TypeDef trigger_type);

void pwm_init(TIM_TypeDef *tim, uint8_t channel, struct gpio *pio, uint32_t high_pulse, uint32_t period);
	
void pwm_set_high_pulse(TIM_TypeDef *tim, uint8_t channel, uint32_t high_pulse);

int platform_timer_intr_init(uint32_t interval);

int platfor_timer_refresh(void);

int platform_counter_init(uint32_t interval);

uint32_t platform_get_counter(void);

void platform_get_uniq_id(uint32_t *uid_buf);
