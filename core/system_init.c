#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_i2c.h"
#include "system_init.h"


volatile uint8_t tx_buf[TX_BUF_SIZE];
volatile uint16_t tx_head = 0;
volatile uint16_t tx_tail = 0;

extern uint8_t frame_buffer[8][128];


void  USART2_Init();

void USART2_SendString_IT(const char *str)
{
    taskENTER_CRITICAL();   

    while (*str)
    {
        uint16_t next = (tx_head + 1) % TX_BUF_SIZE;

        if (next == tx_tail)
        {
            break;
        }

        tx_buf[tx_head] = (uint8_t)(*str++);
        tx_head = next;
    }
    LL_USART_EnableIT_TXE(USART2);

    taskEXIT_CRITICAL();
}
void USART2_IRQHandler(void)
{

    if (LL_USART_IsActiveFlag_TXE(USART2) &&
        LL_USART_IsEnabledIT_TXE(USART2))
    {
        if (tx_tail != tx_head)
        {
            LL_USART_TransmitData8(USART2, tx_buf[tx_tail]);
            tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
        }
        else
        {
            LL_USART_DisableIT_TXE(USART2);
        }
    }
}
void USART2_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_2, LL_GPIO_AF_7);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_2, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_3, LL_GPIO_AF_7);
    LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_3, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_3, LL_GPIO_PULL_UP);

    LL_USART_Disable(USART2);

    LL_USART_SetTransferDirection(USART2, LL_USART_DIRECTION_TX_RX);
    LL_USART_SetParity(USART2, LL_USART_PARITY_NONE);
    LL_USART_SetDataWidth(USART2, LL_USART_DATAWIDTH_8B);
    LL_USART_SetStopBitsLength(USART2, LL_USART_STOPBITS_1);
    LL_USART_SetOverSampling(USART2, LL_USART_OVERSAMPLING_16);

    LL_USART_SetBaudRate(USART2, 16000000, LL_USART_OVERSAMPLING_16, 115200);

    NVIC_SetPriority(USART2_IRQn, 5);
    NVIC_EnableIRQ(USART2_IRQn);
    LL_USART_Enable(USART2);
    
}

void SPI2_Init()
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_13, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_13, LL_GPIO_AF_5);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_13, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_13, LL_GPIO_PULL_NO);


    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_14, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_14, LL_GPIO_AF_5);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_14, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_14, LL_GPIO_PULL_UP);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_15, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOB, LL_GPIO_PIN_15, LL_GPIO_AF_5);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_15, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_15, LL_GPIO_PULL_NO);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_12, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_12, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_12, LL_GPIO_PULL_UP);


    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_12);


    LL_SPI_Disable(SPI2);

    LL_SPI_SetTransferDirection(SPI2, LL_SPI_FULL_DUPLEX);
    LL_SPI_SetMode(SPI2, LL_SPI_MODE_MASTER);
    LL_SPI_SetDataWidth(SPI2, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_SetTransferBitOrder(SPI2, LL_SPI_MSB_FIRST);
    LL_SPI_SetClockPolarity(SPI2, LL_SPI_POLARITY_LOW);
    LL_SPI_SetClockPhase(SPI2, LL_SPI_PHASE_1EDGE);
    LL_SPI_SetBaudRatePrescaler(SPI2, LL_SPI_BAUDRATEPRESCALER_DIV64);
    LL_SPI_SetNSSMode(SPI2, LL_SPI_NSS_SOFT);
    LL_SPI_Enable(SPI2);
}


uint8_t SPI2_Transfer(uint8_t data)
{
    
    while (!LL_SPI_IsActiveFlag_TXE(SPI2));
    LL_SPI_TransmitData8(SPI2, data);

    while (!LL_SPI_IsActiveFlag_RXNE(SPI2));
    return LL_SPI_ReceiveData8(SPI2);
}

void I2C1_Init()
{
  NVIC_SetPriority(I2C1_EV_IRQn, 6);
NVIC_SetPriority(I2C1_ER_IRQn, 6);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C1);
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C1);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_6, LL_GPIO_AF_4);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_UP);


    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOB, LL_GPIO_PIN_7, LL_GPIO_AF_4);
    LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_VERY_HIGH);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_UP);

    LL_I2C_Disable(I2C1);
    LL_I2C_ConfigSpeed(I2C1, 16000000u, 400000u, LL_I2C_DUTYCYCLE_2);
    I2C1->CCR |= I2C_CCR_FS;
    LL_I2C_SetMode(I2C1, LL_I2C_MODE_I2C);
    LL_I2C_SetOwnAddress1(I2C1, 0, LL_I2C_OWNADDRESS1_7BIT);
    LL_I2C_Enable(I2C1);
}

void I2C1_Master_SendByte(uint8_t slave_addr, uint8_t data)
{
  
    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1));
    LL_I2C_TransmitData8(I2C1, slave_addr << 1);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C1));
    (void)I2C1->SR1;   
    (void)I2C1->SR2;   

    while(!LL_I2C_IsActiveFlag_TXE(I2C1));
    LL_I2C_TransmitData8(I2C1, data);
    while(!LL_I2C_IsActiveFlag_BTF(I2C1));
    LL_I2C_GenerateStopCondition(I2C1);
}
void send_i2c_data(uint8_t data)
{
   while(!LL_I2C_IsActiveFlag_TXE(I2C1));
    LL_I2C_TransmitData8(I2C1, data);

    

}

void i2c1_start()
{
   
    LL_I2C_GenerateStartCondition(I2C1);

    while(!LL_I2C_IsActiveFlag_SB(I2C1));   

    LL_I2C_TransmitData8(I2C1, 0x3C << 1);


    while(!LL_I2C_IsActiveFlag_ADDR(I2C1)); 

     (void)I2C1->SR1; 
     (void)I2C1->SR2; 
}
void i2c1_stop()
{
    while(!LL_I2C_IsActiveFlag_BTF(I2C1));  
    LL_I2C_GenerateStopCondition(I2C1);
   

}


void DMA1_I2C1_TX_Init()
{
 
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_6);



    LL_DMA_SetChannelSelection(DMA1,  LL_DMA_STREAM_6,LL_DMA_CHANNEL_1);

    LL_DMA_SetDataTransferDirection(DMA1,LL_DMA_STREAM_6,LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

    LL_DMA_SetPeriphAddress(DMA1,  LL_DMA_STREAM_6, (uint32_t)&I2C1->DR);


    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_6,LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphIncMode(DMA1,LL_DMA_STREAM_6,LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_6,LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_6, LL_DMA_PDATAALIGN_BYTE);

  
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_6);
    LL_DMA_SetMode(DMA1, LL_DMA_STREAM_6, LL_DMA_MODE_NORMAL);
    LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_6, LL_DMA_PRIORITY_MEDIUM);
    LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_6);

    NVIC_SetPriority(DMA1_Stream6_IRQn, 6);
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

void DMA1_I2C1_TX_Start(uint8_t page)
{
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_6);
    while (LL_DMA_IsEnabledStream(DMA1, LL_DMA_STREAM_6));
    LL_DMA_SetMemoryAddress(DMA1,LL_DMA_STREAM_6,(uint32_t)&frame_buffer[page][0]);

    LL_DMA_SetDataLength(DMA1,LL_DMA_STREAM_6,128);

    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_6);
    LL_I2C_EnableDMAReq_TX(I2C1);
}
void DMA1_Stream6_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC6(DMA1))
    {
        LL_DMA_ClearFlag_TC6(DMA1);
        LL_I2C_DisableDMAReq_TX(I2C1);
        i2c1_stop();
    }
}
void gfx_loading_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height,uint8_t percent)
{
    gfx_rect(x, y, width, height);
    uint8_t fill_w = percent % width;
    gfx_rect_fill(x, y, height,fill_w);
}

 void Button_GPIO_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

    uint32_t pins[] = {LL_GPIO_PIN_0,LL_GPIO_PIN_1,LL_GPIO_PIN_2,LL_GPIO_PIN_3,LL_GPIO_PIN_4,LL_GPIO_PIN_5};

    for(int i = 0; i < 6; i++)
    {
        LL_GPIO_SetPinMode(GPIOC, pins[i], LL_GPIO_MODE_INPUT);
        LL_GPIO_SetPinPull(GPIOC, pins[i], LL_GPIO_PULL_UP);
    }
}
