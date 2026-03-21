#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include <stdlib.h>
#include"system_init.h"
#include <stdlib.h>
#include "queue.h"
#include "snake.h"
#include "semphr.h"

SemaphoreHandle_t i2cMutex;
#define MAX_IDLE_COUNT 288238  //calibrated value

#define LOG_ENABLE 0
#define LOG_LEVEL 1
volatile uint32_t idleCounter;

extern uint8_t monitor_icon[],settings_icon[],game_icon[];
TaskHandle_t menuTaskHandle,ButtonTaskHandle,bootTaskHandle,SnakeTaskHandle,systemTaskHandle,i2cOwnerTask,IdleCalibrateTaskHandle;

  QueueHandle_t buttonQueue;

uint8_t total_apps = 2;


enum console_aplication
{
   system_monitor,
   games,
   settings
};

void snake_game(void *arg);

void boot_task(void *arg)
{
   USART2_Init();
   I2C1_Init();
   sh1106_init();
   DMA1_I2C1_TX_Init();
   LOG("SYSTEM BOOTED\n");

   Button_GPIO_Init();
   
   gfx_clear();

vTaskResume(menuTaskHandle);
vTaskResume(ButtonTaskHandle);


    
   vTaskDelete(NULL);
}
 

void draw_app_icon()
{

   gfx_draw_snake_gif();
}
void menu_task(void *arg)
{
   srand(xTaskGetTickCount());
   ButtonEvent event;
   char buf[30];
   int pos = 0;
   draw_app_icon();
   
  while(1)
  { 
   draw_app_icon();
   if(xQueueReceive(buttonQueue, &event, 0) == pdTRUE)
   {
      #if LOG_ENABLE
      buf[pos++] = 'B';
      buf[pos++] = '0' + event.button;
      buf[pos++] = ' ';
      buf[pos++] = 'S';
      buf[pos++] = '=';
      buf[pos++] = '0' + event.state;
      buf[pos++] = '\n';
      buf[pos] = '\0';

      
      #endif
      if(event.state == BUTTON_PRESSED)
      {
      if(event.button == BUTTON_SELECT)
      {
               srand(xTaskGetTickCount());
               xTaskCreate(snake_game,"snake",1000,NULL,3,&SnakeTaskHandle);
               vTaskResume(systemTaskHandle);
               vTaskSuspend(menuTaskHandle);
            
            
      }
   }
   }
   }
}
void gfx_update_safe()
{
     xSemaphoreTake(i2cMutex, portMAX_DELAY);
      i2cOwnerTask = xTaskGetCurrentTaskHandle();
      gfx_update();           
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  
      xSemaphoreGive(i2cMutex);
}
void snake_game(void *arg)
{
   

   struct snake s1;
   struct food_coordinate f1;
   struct game_stat st;

   spawn_snake(&s1);
   spawn_food(&f1,&s1);

   ButtonEvent event;

   TickType_t lastWakeTime;
   const TickType_t frameDelay = pdMS_TO_TICKS(120);
   lastWakeTime = xTaskGetTickCount();

   char buf[32];
   gfx_clear();
   xQueueReset(buttonQueue);
   for(;;)
   {
      while(xQueueReceive(buttonQueue, &event, 0) == pdTRUE)
      {
       

            if(event.state == BUTTON_PRESSED)
            {
               if(event.button == BUTTON_LEFT  && s1.direction != snake_RIGHT)
               {
                  s1.direction = snake_LEFT;
               }
               else if(event.button == BUTTON_RIGHT && s1.direction != snake_LEFT)
               {
                  s1.direction = snake_RIGHT;
               }
               else if(event.button == BUTTON_UP && s1.direction != snake_DOWN)
               {
                  s1.direction = snake_UP;
               }
               else if(event.button == BUTTON_DOWN && s1.direction != snake_UP)
               {
                  s1.direction = snake_DOWN;
               }
            }
      }

    
      
      erase_snake(&s1);
      update_snake(&s1);
      check_food_eaten(&f1, &s1);
      draw_snake(&s1);
      draw_food(&f1);
      update_current_score();
      if(check_crash(&s1) == game_over)
      {
            if(draw_game_status()==1)
            {
               lastWakeTime = xTaskGetTickCount(); 
               spawn_snake(&s1);
            }
      }
      else
      {
           draw_obstacle();
      }
     gfx_update_safe();

      vTaskDelayUntil(&lastWakeTime, frameDelay);
   }
}
void button_Task(void *arg)
{
    uint8_t button_state[6] = {0};
    TickType_t button_tick[6] = {0};

    uint32_t button_pin[6] =
    {
        LL_GPIO_PIN_6,
        LL_GPIO_PIN_1,
        LL_GPIO_PIN_2,
        LL_GPIO_PIN_3,
        LL_GPIO_PIN_4,
        LL_GPIO_PIN_5
    };

    char buf[32];
    int pos = 0;
    TickType_t last_print = 0;

    ButtonEvent instance;
    
    while(1)
    {
        TickType_t now = xTaskGetTickCount();

        for(uint8_t i = 0; i < 6; i++)
        {
            if(button_state[i] == BUTTON_IDLE &&
               !LL_GPIO_IsInputPinSet(GPIOC, button_pin[i]))
            {
                button_state[i] = BUTTON_PRESSED;
                button_tick[i] = now;

                instance.button = i;
                instance.state = BUTTON_PRESSED;
                
                xQueueSend(buttonQueue, &instance, 0);
            }
            else if(button_state[i] == BUTTON_PRESSED &&
                    now - button_tick[i] > pdMS_TO_TICKS(300) &&
                    !LL_GPIO_IsInputPinSet(GPIOC, button_pin[i]))
            {
                button_state[i] = BUTTON_HELD;

                instance.button = i;
                instance.state = BUTTON_HELD;

                xQueueSend(buttonQueue, &instance, 0);
            }
            else if(button_state[i] != BUTTON_IDLE &&
            LL_GPIO_IsInputPinSet(GPIOC, button_pin[i]) &&
            now - button_tick[i] > pdMS_TO_TICKS(20))
            {
               button_state[i] = BUTTON_IDLE;
            }
        }

#if LOG_ENABLE
        if(now - last_print > pdMS_TO_TICKS(300))
        {
            pos = 0;

            for(int i = 0; i < 6; i++)
            {
                buf[pos++] = 'B';
                buf[pos++] = '0' + i;
                buf[pos++] = '=';
                buf[pos++] = '0' + button_state[i];
                buf[pos++] = ' ';
            }

            buf[pos++] = '\n';
            buf[pos] = '\0';

            LOG(buf);

            last_print = now;
        }
#endif


        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
void System_monitor(void *arg)
{
    uint32_t idle;
    uint32_t cpu;
    uint8_t buf[20];

    size_t heap;

    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));   

        idle = idleCounter;
        idleCounter = 0;                  

        cpu = 100 - ((idle * 100) / MAX_IDLE_COUNT);
        
         heap = xPortGetFreeHeapSize();
        int_to_string(cpu, buf);
        gfx_drawText(0,0,FONT0,"CPU=");
        gfx_drawText(0,20,FONT0,buf);
         gfx_drawText(0,35,FONT0,"%");
        int_to_string(heap, buf);
        //gfx_drawText(0,20,FONT0,buf);
        gfx_update_safe();
    }
}


void vApplicationIdleHook(void)
{
    idleCounter++;
}
void IdleCalibrateTask(void *arg)
{
    uint8_t buf[20];

    vTaskDelay(pdMS_TO_TICKS(1000));   

    int_to_string(idleCounter, buf);

    LOG("MAX_IDLE_COUNT: ");
    LOG(buf);
    LOG("\n");

    while(1);
}

int main(void)
{
   
  
   buttonQueue = xQueueCreate(10, sizeof(ButtonEvent));
   xTaskCreate(IdleCalibrateTask,"idle_hook",300,NULL,4,&IdleCalibrateTaskHandle);
   xTaskCreate(boot_task,"boot",300,NULL,4,&bootTaskHandle);
   xTaskCreate(menu_task,"menu",300,NULL,3,&menuTaskHandle);
   xTaskCreate(button_Task,"button",300,NULL,2,&ButtonTaskHandle);
   xTaskCreate(System_monitor,"System",200,NULL,1,&systemTaskHandle);
   
   i2cMutex = xSemaphoreCreateMutex();
   if(i2cMutex == NULL)
   {
      while(1); 
   }
   
   vTaskSuspend(IdleCalibrateTaskHandle);
   vTaskSuspend(menuTaskHandle);
   vTaskSuspend(ButtonTaskHandle);
   vTaskSuspend(systemTaskHandle);
   vTaskStartScheduler();
 
}




