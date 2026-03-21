#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H
#include<stdint.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include <stdlib.h>
#include"system_init.h"
#include <stdlib.h>
#include "queue.h"

#define SCREEN_ROWS 64
#define SCREEN_COLS 128

#define BLOCK_SIZE 4

#define GRID_ROWS (SCREEN_ROWS / BLOCK_SIZE)
#define GRID_COLS (SCREEN_COLS / BLOCK_SIZE)

#define MAX_SNAKE_LENGTH 50
#define SNAKE_SPAWN_LENGTH 5

#define MIN_ROW 1
#define MAX_ROW (GRID_ROWS - 2)

#define MIN_COL 1
#define MAX_COL (GRID_COLS - 2)

struct snake
{
    struct coordinate
    {
        uint8_t row;
        uint8_t col;
    } body[MAX_SNAKE_LENGTH];

    uint8_t length;
    uint8_t direction;
};

enum snake_direction
{
   snake_LEFT,
   snake_RIGHT,
   snake_UP,
   snake_DOWN,
   snake_NOCHANGE
};

struct food_coordinate
{
   uint8_t row;
   uint8_t col;
};

struct obstacle_coordinate
{
   uint8_t row;
   uint8_t col;
};
struct obstacle_line
{
    uint8_t row;
    uint8_t col;
    uint8_t length;
};
struct game_stat
{
   uint32_t high_score;
   uint8_t current_score;
   uint8_t game_level;
};

enum game_status
{
   game_over =0,
   game_running =1
};

extern uint32_t score;
void spawn_snake(struct snake *s1);
void update_snake(struct snake *s1);
void spawn_food(struct food_coordinate *food, struct snake *s1);
void check_food_eaten(struct food_coordinate *food, struct snake *s1);
void erase_snake(struct snake *s1);
void draw_snake(struct snake *s1);
void draw_food( struct food_coordinate *f1);
uint8_t check_crash(struct snake *s1);
void update_current_score();
void draw_obstacle(void);
uint8_t  draw_game_status();


#endif