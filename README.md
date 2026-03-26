# BITCONSOLE – Embedded Graphics Console (STM32 + FreeRTOS)

A bare-metal + FreeRTOS based embedded graphics console built on STM32, featuring a custom graphics stack, task-based architecture, and interactive applications like Snake.

---

## Overview

BITCONSOLE is a small embedded system that simulates a handheld console environment with:

- Task-based architecture using FreeRTOS  
- Custom graphics rendering on SH1106 OLED  
- Button-driven UI system  
- Game support (Snake implemented)  
- Low-level peripheral control using STM32 LL drivers  

This project focuses on firmware architecture, concurrency, and low-level driver design rather than using HAL or auto-generated code.

---

## Features

- FreeRTOS-based multitasking
- SH1106 OLED graphics driver (framebuffer-based)
- Custom graphics API (text, shapes, bitmaps)
- Button input system with debouncing + events
- Snake game implementation
- Interrupt-driven UART logging
- I2C + DMA based display updates (with synchronization)
- Mutex-protected shared peripherals

---

## Architecture

### Tasks

- Boot Task – Initializes peripherals and system
- Menu Task – Displays UI and launches apps
- Button Task – Handles input and debouncing
- Snake Task – Game loop and rendering

---

## Inter-Task Communication

- Queue (buttonQueue) – Button events
- Mutex (i2cMutex) – Protects I2C access
- Task Notification – Sync after display update

---

## Graphics System

- Framebuffer: `frame_buffer[8][128]`
- Pixel-level rendering
- Bitmap + text drawing

### APIs

- `gfx_set_pixel()`
- `gfx_drawText()`
- `gfx_rect_fill()`
- `gfx_draw_bitmap()`
- `gfx_update()`

---

## Peripheral Drivers

### USART (Interrupt Driven)

- Ring buffer TX
- Non-blocking logging:
```c
LOG("message");
```

### I2C

- 400kHz Fast Mode
- Used for OLED communication (DMA supported)


---

## Snake Game

- Grid-based movement
- Food spawning
- Collision detection
- Score + High score system

Game loop:
Input → Update → Render → Delay

---

## Input System

- GPIO button scanning
- Debounce + hold detection
- Event-driven design

---

## Concurrency Design

```c
void gfx_update_safe()
{
    xSemaphoreTake(i2cMutex, portMAX_DELAY);
    gfx_update();
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    xSemaphoreGive(i2cMutex);
}
```

---

## Hardware

- STM32 (F4 series recommended)
- SH1106 OLED (128x64)
- GPIO Buttons
- UART for debugging

---

## How to Run

1. Open in STM32CubeIDE (build only)
2. Flash to STM32 board
3. Connect OLED via I2C
4. Use buttons to navigate and play

---

## Design Philosophy

- No HAL (LL + register-level)
- Clean modular design
- Practical RTOS usage
- Focus on real firmware patterns

---

## Future Improvements

- More apps (clock, notes, settings)
- SD card + FATFS
- UI improvements
- Sound support
- Advanced scheduler

---


