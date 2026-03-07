#include<stdint.h>
#include"snake.h"


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
    int valid;

    do
    {
        valid = 1;

        food->row = (rand() % GRID_ROWS);
        food->col = (rand() % GRID_COLS);

 
        for(int i=0;i<s1->length;i++)
        {
            if(food->row == s1->body[i].row &&
               food->col == s1->body[i].col)
            {
                valid = 0;
                break;
            }
        }

    } while(!valid);
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
    gfx_rect_fill(f1->row * BLOCK_SIZE,f1->col * BLOCK_SIZE,BLOCK_SIZE, BLOCK_SIZE);
}
void draw_obstacle(struct obstacle_coordinate)
{
    
}