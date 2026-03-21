#include<stdint.h>
#include"snake.h"
#include"system_init.h"

uint32_t current_score=0;
uint32_t high_score =0;
char current_score_str[50];
char high_score_str[50];

extern  QueueHandle_t buttonQueue;
extern TaskHandle_t menuTaskHandle,systemTaskHandle;
void spawn_snake(struct snake *s1)
{
    s1->length = SNAKE_SPAWN_LENGTH;

    uint8_t start_row = GRID_ROWS / 2;
    uint8_t start_col = GRID_COLS / 2;

    s1->body[0].row = start_row;
    s1->body[0].col = start_col;

    s1->direction = snake_RIGHT;
}
void update_snake(struct snake *s1)
{
    uint8_t prev_row = s1->body[0].row;
    uint8_t prev_col = s1->body[0].col;

    switch (s1->direction)
    {
        case snake_LEFT:
            s1->body[0].col = (s1->body[0].col + GRID_COLS - 1) % GRID_COLS;
            break;

        case snake_RIGHT:
            s1->body[0].col = (s1->body[0].col + 1) % GRID_COLS;
            break;

        case snake_UP:
            s1->body[0].row = (s1->body[0].row + GRID_ROWS - 1) % GRID_ROWS;
            break;

        case snake_DOWN:
            s1->body[0].row = (s1->body[0].row + 1) % GRID_ROWS;
            break;
    }

    uint8_t temprow, tempcol;

    for(int i = 1; i < s1->length; i++)
    {
        temprow = s1->body[i].row;
        tempcol = s1->body[i].col;

        s1->body[i].row = prev_row;
        s1->body[i].col = prev_col;

        prev_row = temprow;
        prev_col = tempcol;
    }
}


void spawn_food(struct food_coordinate *food, struct snake *s1)
{
    
uint8_t valid = 0;

while(!valid)
{
    food->row = rand() % GRID_ROWS;
    food->col = rand() % GRID_COLS;

    valid = 1;

    for(int i = 0; i < s1->length; i++)
    {
        if(food->row == s1->body[i].row &&
           food->col == s1->body[i].col)
        {
            valid = 0;
            break;
        }
    }
    
        if(food->row*BLOCK_SIZE < 10)
        {
            valid =0;
       
        }
        if(food->row*BLOCK_SIZE >58)
        {
            valid =0;
         
        }
    
   

}
}


void check_food_eaten(struct food_coordinate *food, struct snake *s1)
{
    if(s1->body[0].row == food->row &&s1->body[0].col == food->col)
    {
        if(s1->length < MAX_SNAKE_LENGTH)
        {
            s1->body[s1->length] = s1->body[s1->length - 1];
            s1->length++;
        }
        current_score++;
        spawn_food(food, s1);
    }
}

void erase_snake(struct snake *s1)
{
      for(int i = 0; i < s1->length; i++)
   {
      uint8_t x = s1->body[i].row * BLOCK_SIZE;
      uint8_t y = s1->body[i].col * BLOCK_SIZE;

      gfx_erase_area(x, y, BLOCK_SIZE, BLOCK_SIZE);
   }
}

void draw_snake(struct snake *s1)
{
        for(int i = 0; i < s1->length; i++)
    {
        uint8_t x = s1->body[i].row * BLOCK_SIZE;
        uint8_t y = s1->body[i].col * BLOCK_SIZE;

        gfx_rect_fill(x, y, BLOCK_SIZE, BLOCK_SIZE);
}
}
void draw_food(struct food_coordinate *f1)
{
    // Convert grid coordinates to pixel coordinates
    gfx_rect_fill(f1->row * BLOCK_SIZE,f1->col * BLOCK_SIZE,BLOCK_SIZE, BLOCK_SIZE);
}
void draw_obstacle(void)
{
    
  gfx_draw_dot_line(8,0,8,127);
  gfx_draw_dot_line(59,0,59,127);
}
uint8_t check_crash(struct snake *s1)
{
  
      uint8_t px = s1->body[0].row * BLOCK_SIZE;
    for(int i=1;i < s1->length ;i++)
    {
        if(s1->body[0].row== s1->body[i].row && s1->body[0].col == s1->body[i].col)
        {
            LOG("CRASHED");
            return game_over;
        }
    } 
    if(px <= 7 || px >= 60)
    {
        LOG("CRASHED");
       return game_over;
    }
    return game_running;
}

void update_current_score()
{    
    
    
     gfx_erase_area(0,80,8,40);
     int_to_string(current_score,current_score_str);
     gfx_drawText(0,80,FONT0,"SCORE: ");
     gfx_drawText(0,110,FONT0,current_score_str);

}
uint8_t  draw_game_status()
{
    vTaskSuspend(systemTaskHandle);
    if(current_score > high_score)
    {
        high_score = current_score;
    }
    gfx_clear();
    uint8_t select =1;
    
    gfx_rect(0,0,60,60);
    gfx_drawText(10,5,FONT0,"++SCORE++");
    int_to_string(current_score,current_score_str);
    gfx_drawText(20,20,FONT0,current_score_str);
    gfx_drawText(38,5,FONT0,"HIGH SCORE");
    int_to_string(high_score,high_score_str);
    gfx_drawText(45,20,FONT0,high_score_str);
    gfx_update_safe();

    gfx_drawText(12,65,FONT1,"PLAY AGAIN");
    gfx_rect(10,62,65,12);
    gfx_drawText(27,80,FONT1,"EXIT");
    gfx_rect(26,62,65,12);
    gfx_invert_area(10,62,11,65);
  
    ButtonEvent event;

    while (1)
    {
       gfx_invert_area(0,0,60,60);
       vTaskDelay(pdMS_TO_TICKS(100));
       gfx_update_safe();
        gfx_invert_area(0,0,60,60);
       vTaskDelay(pdMS_TO_TICKS(100));
       
       
       while(xQueueReceive(buttonQueue, &event, 0) == pdTRUE)
      {
            if(event.state == BUTTON_PRESSED)
            {
               
               if(event.button == BUTTON_UP)
               {
                   gfx_invert_area(10,62,11,65);
                   gfx_invert_area(26,62,11,65);
                 select = 1;
               }
               else if(event.button == BUTTON_DOWN )
               {
                   gfx_invert_area(10,62,11,65);
                   gfx_invert_area(26,62,11,65);
                   select = 2;
               }
               else if(event.button == BUTTON_SELECT )
               {
                  if(select == 1)
                  {
                    current_score = 0;
                    gfx_clear();
                    vTaskResume(systemTaskHandle);
                    return 1;
                  }
                  else
                  {
                    current_score = 0;
                    gfx_clear();
                    gfx_update_safe(); 
                    xQueueReset(buttonQueue);
                    vTaskSuspend(systemTaskHandle);
                    vTaskResume(menuTaskHandle);
                    vTaskDelete(NULL);
                  }

               }
            }
      }
          
        gfx_update_safe();  

    }
    
    return 0;
}