# BITCONSOLE – Embedded Graphics Console (STM32 + FreeRTOS)

A FreeRTOS based embedded graphics console built on STM32, featuring a custom graphics stack, task-based architecture, and interactive applications like Snake.



![BITCONSOLE Demo](https://github.com/arjun4embed/Bitconsole/blob/master/Media/gif2.gif)
---

## Overview

BITCONSOLE is a small embedded system that simulates a handheld console environment with:

- Task-based architecture using FreeRTOS  
- Custom graphics rendering on SH1106 OLED  
- Button-driven UI system  
- Game support (Snake implemented)  
- Low-level peripheral control using STM32 LL drivers  



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
- System Task - Gives CPU usage

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
## System Monitoring

The system includes a basic CPU usage monitoring mechanism using the FreeRTOS idle task.

- CPU usage is calculated based on idle task execution count
- Uses calibrated `MAX_IDLE_COUNT` for 100% idle reference
- Helps visualize system load during runtime

### Concept

CPU Usage ≈  
`(1 - (idleCounter / MAX_IDLE_COUNT)) * 100`

- `idleCounter` increments inside idle task
- Lower idle time → higher CPU usage

This demonstrates real-time system performance monitoring in an RTOS environment.
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

## Hardware

- STM32 F446RE    
- SH1106 OLED (128x64)
- GPIO Buttons
- UART for debugging

---



## Future Improvements

- More apps (clock, notes, settings)
- SD card + FATFS
- UI improvements
- Sound support
- Advanced scheduler

---


