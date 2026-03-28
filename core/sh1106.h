#ifndef SH1106_H
#define SH1106_H
#include<stdint.h>


enum font{
    FONT0,
    FONT1,
    FONT2,
    FONT3,
    FONT4
};
enum char_type{
    INVERT,
    NON_INVERT
};

void gfx_update();
void sh1106_init();
void gfx_clear();
void gfx_fill();
uint8_t gfx_read_pixel(uint8_t row, uint8_t column);
void gfx_set_pixel(uint8_t row, uint8_t column);
void gfx_clear_pixel(uint8_t row, uint8_t column);
void gfx_draw_line(uint8_t x1_point,uint8_t y1_point,uint8_t x2_point,uint8_t y2_point);
void gfx_rect(uint8_t x_point, uint8_t y_point, uint8_t width, uint8_t height);
void gfx_rect_fill(uint8_t x_point, uint8_t y_point, uint8_t width, uint8_t height);
void gfx_drawChar(uint8_t x_point, uint8_t y_point, char c);
void gfx_drawChar3x5(uint8_t x_point, uint8_t y_point, char c);
void gfx_drawText(uint8_t x_point, uint8_t y_point,uint8_t scale,const char *str);
void gfx_drawCharBig_1(uint8_t x, uint8_t y, char c);
void gfx_drawCharBig_2(uint8_t x, uint8_t y, char c);
void gfx_drawCharMed(uint8_t x_point, uint8_t y_point, char c);
void gfx_invert_area(uint8_t x_point, uint8_t y_point, uint8_t width, uint8_t height);
void gfx_draw_bitmap(uint8_t x, uint8_t y,const uint8_t *bitmap,uint8_t width,uint8_t height);
void gfx_draw_bitconsole_logo();
void gfx_draw_icon16(uint8_t x, uint8_t y, const uint8_t *icon);
void gfx_draw_icon30x24(uint8_t y, uint8_t x, const uint8_t *icon);
void gfx_erase_area(uint8_t x_point, uint8_t y_point, uint8_t width, uint8_t height);
void gfx_draw_snake_gif();
void gfx_draw_dot_line(uint8_t x1_point,uint8_t y1_point,uint8_t x2_point,uint8_t y2_point);
void gfx_loading_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height,uint8_t percent);

#endif