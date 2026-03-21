
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
#include "semphr.h"



#define LOG(str)   USART2_SendString_IT(str)
#define TX_BUF_SIZE 128


enum button_state
{
   BUTTON_IDLE          =0,
   BUTTON_PRESSED       =1,
   BUTTON_HELD          =2,
   BUTTON_RELEASED      =3,
   BUTTON_PRESSED_10MS =4,
   BUTTON_PRESSED_50MS  =5,
   BUTTON_PRESSED_100MS  =6
};

enum button_direction
{
   BUTTON_DOWN       =0,
   BUTTON_UP         =3,
   BUTTON_LEFT       =2,
   BUTTON_RIGHT      =1,
   BUTTON_SELECT     =5,
   BUTTON_SELECT2    =4
};
typedef struct
{
    uint8_t button;
    uint8_t state;
} ButtonEvent;




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


void int_to_string(int num, char *str);
void log_uint(size_t val);


void gfx_update_safe();

void tim2_init();
void buzzer_stop();
void buzzer_play(uint16_t freq);
#endif 