# ===============================
# Toolchain
# ===============================

CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

# ===============================
# MCU Configuration
# ===============================

MCU     = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# ===============================
# Compiler Flags
# ===============================

CFLAGS  = $(MCU) -std=gnu11 -O0 -g3 -Wall \
           -ffunction-sections -fdata-sections

CFLAGS += -DSTM32F446xx
CFLAGS += -DUSE_FULL_LL_DRIVER


# Include paths
CFLAGS +=  \
-I./core \
-I./FreeRTOS \
-I./FreeRTOS/include \
-I./FreeRTOS/portable \
-I./Config\
-I./Driver/CMSIS/Core/Include \
-I./Driver/CMSIS/Device/ST/STM32F4xx/Include \
-I./Driver/LL_driver \
-I./core/App/Games \
-I./core/


# ===============================
# Linker Flags
# ===============================

LDFLAGS = $(MCU) \
          -T core/STM32F446RETX_FLASH.ld \
          -Wl,--gc-sections \
          -Wl,-Map=output.map

# ===============================
# Source Files
# ===============================

SRCS = \
core/main.c \
core/sh1106.c \
core/App/Games/snake.c \
FreeRTOS/tasks.c \
FreeRTOS/queue.c \
FreeRTOS/list.c \
FreeRTOS/portable/port.c \
FreeRTOS/portable/heap_4.c \
Driver/LL_driver/stm32f4xx_ll_gpio.c \
Driver/LL_driver/stm32f4xx_ll_rcc.c \
Driver\CMSIS\Device\ST\STM32F4xx\Source\Templates\system_stm32f4xx.c \
Driver/LL_driver/stm32f4xx_ll_usart.c \
Driver/LL_driver/stm32f4xx_ll_spi.c \
Driver/LL_driver/stm32f4xx_ll_i2c.c \
Driver/LL_driver/stm32f4xx_ll_dma.c \
core/system_init.c 

ASMS = core/startup_stm32f446retx.s

OBJS = $(SRCS:.c=.o) $(ASMS:.s=.o)

# ===============================
# Target
# ===============================

TARGET = output.elf

all: $(TARGET)

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble startup file
%.o: %.s
	$(AS) $(MCU) -c $< -o $@

# Link
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)
	$(SIZE) $(TARGET)

# Flash (requires st-flash installed)
flash: $(TARGET)
	openocd -f interface/stlink.cfg \
	         -f target/stm32f4x.cfg \
	         -c "program $(TARGET) verify reset exit"


# Clean
clean:
	rm -f $(OBJS) $(TARGET) output.map
