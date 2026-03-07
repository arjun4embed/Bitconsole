
#ifndef CORE_SYSTEM_INIT_H
#define CORE_SYSTEM_INIT_H
#include <stdint.h>
#include "sh1106.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_dma2d.h"



#define LOG(str)   USART2_SendString_IT(str)
#define TX_BUF_SIZE 128

void  USART2_Init();
void USART2_SendString_IT(const char *str);
void UART_PrintHex(uint8_t value);

 void Button_GPIO_Init(void);

uint8_t SPI2_Transfer(uint8_t data);
void SPI2_Init();

void I2C1_Init();
void i2c1_start();
void i2c1_stop();
void send_i2c_data(uint8_t data);

void DMA1_I2C1_TX_Init();
void DMA1_I2C1_TX_Start(uint8_t page);


void gfx_loading_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height,uint8_t percent);


#endif 