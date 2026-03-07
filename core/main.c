#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include <stdlib.h>
#include"system_init.h"
#include <stdlib.h>
#include "queue.h"
#include "snake.h"

#define LOG_ENABLE 1
#define LOG_LEVEL 1


extern uint8_t monitor_icon[],settings_icon[],game_icon[];
TaskHandle_t menuTaskHandle,ButtonTaskHandle,bootTaskHandle,SnakeTaskHandle;

  QueueHandle_t buttonQueue;



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

void boot_task(void *arg)
{
  
   I2C1_Init();
   sh1106_init();
   USART2_Init();
   LOG("SYSTEM BOOTED\n");

   Button_GPIO_Init();
   DMA1_I2C1_TX_Init();
   gfx_clear();

gfx_draw_bitconsole_logo();

gfx_update();


   for(int p = 0; p <= 100; p++)
   {
      gfx_loading_bar(55, 10, 100, 2, p);
      gfx_update();
      vTaskDelay(pdMS_TO_TICKS(1));
   }

   gfx_clear();
  //vTaskResume(SnakeTaskHandle);
 vTaskResume(ButtonTaskHandle);
    
   vTaskDelete(NULL);
}
void write_menu_header(uint8_t current_rowvalue)
{
   gfx_erase_area(0,0,9,127);

   switch(current_rowvalue)
   {
      case 0:
       gfx_drawText(0,0,FONT1,"SYSTEM MONITOR");
       break;
      case 40:

      gfx_drawText(0,0,FONT1,"GAMES");
       break;
       case 80:
       gfx_drawText(0,0,FONT1,"SETTINGS");
       break;
       default:
       
   }
   
}
void menu_task(void *arg)
{
   srand(xTaskGetTickCount());
   ButtonEvent event;
   char buf[32];
   int pos = 0;
   uint8_t current_rowvalue=0, prev_rowvalue;
   gfx_drawText(0,0,FONT1,"SYSTEM MONITOR");
   gfx_draw_line(10,0,10,127);
   gfx_draw_icon30x24(20, 0, monitor_icon);
   gfx_draw_icon30x24(20, 40, game_icon);
   gfx_draw_icon30x24(20, 80, settings_icon);
   gfx_invert_area(15, 0,25,30);
   gfx_update();
  while(1)
  { 
   
                  

         if(xQueueReceive(buttonQueue, &event, portMAX_DELAY) == pdTRUE)
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

            //LOG(buf);
            #endif
            int pos = 0;
            prev_rowvalue = current_rowvalue;
            if(event.button == BUTTON_LEFT && event.state == BUTTON_PRESSED_10MS)
            {
               gfx_invert_area(15, prev_rowvalue,25,30);
               current_rowvalue = (current_rowvalue + 80) % 120;
               gfx_invert_area(15, current_rowvalue,25,30);
                write_menu_header(current_rowvalue);
               gfx_update();
            }
            else if(event.button == BUTTON_RIGHT && event.state == BUTTON_PRESSED_10MS)
            {
               gfx_invert_area(15, prev_rowvalue,25,30);
               current_rowvalue = (current_rowvalue + 40) % 120;
               gfx_invert_area(15, current_rowvalue,25,30);
               write_menu_header(current_rowvalue);
               gfx_update();

            }
            
         }
   }
}

void snake_game(void *arg)
{
   

   struct snake s1;
   struct food_coordinate f1;

   spawn_snake(&s1);
   spawn_food(&f1,&s1);

   ButtonEvent event;
   char buf[32];
   gfx_clear();
   for(;;)
   {
      while(xQueueReceive(buttonQueue, &event, 0) == pdTRUE)
      {
            int pos = 0;

            buf[pos++] = 'B';
            buf[pos++] = '0' + event.button;
            buf[pos++] = ' ';
            buf[pos++] = 'S';
            buf[pos++] = '=';
            buf[pos++] = '0' + event.state;
            buf[pos++] = '\n';
            buf[pos] = '\0';

            LOG(buf);

            if(event.state == BUTTON_PRESSED_10MS)
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
      gfx_drawText(0,0,FONT0,"HELLO");
      gfx_update();

      vTaskDelay(pdMS_TO_TICKS(100));
   }
}
void button_Task(void *arg)
{
    uint8_t button_state[6] = {0};
    TickType_t button_tick[6] = {0};

    uint32_t button_pin[6] =
    {
        LL_GPIO_PIN_0,
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
                    now - button_tick[i] > pdMS_TO_TICKS(10) &&
                    !LL_GPIO_IsInputPinSet(GPIOC, button_pin[i]))
            {
                button_state[i] = BUTTON_PRESSED_10MS;

                instance.button = i;
                instance.state = BUTTON_PRESSED_10MS;

                xQueueSend(buttonQueue, &instance, 0);
            }
            else if(button_state[i] == BUTTON_PRESSED_10MS &&
                    now - button_tick[i] > pdMS_TO_TICKS(300) &&
                    !LL_GPIO_IsInputPinSet(GPIOC, button_pin[i]))
            {
                button_state[i] = BUTTON_HELD;

                instance.button = i;
                instance.state = BUTTON_HELD;

                xQueueSend(buttonQueue, &instance, 0);
            }
            else if(LL_GPIO_IsInputPinSet(GPIOC, button_pin[i]))
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


        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
int main(void)
{
   srand(xTaskGetTickCount());
   
   buttonQueue = xQueueCreate(10, sizeof(ButtonEvent));
   xTaskCreate(boot_task,"boot",300,NULL,4,&bootTaskHandle);
   xTaskCreate(menu_task,"menu",300,NULL,3,&menuTaskHandle);
   xTaskCreate(button_Task,"button",300,NULL,1,&ButtonTaskHandle);
   xTaskCreate(snake_game,"snake",1000,NULL,3,&SnakeTaskHandle);

   vTaskSuspend(menuTaskHandle);
   vTaskSuspend(ButtonTaskHandle);
   vTaskSuspend(SnakeTaskHandle);
   vTaskStartScheduler();
}




