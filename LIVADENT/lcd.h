#include <lvgl.h>
#include <TFT_eSPI.h>

#include "event_handler.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define DRAW_BUF_LINES 30

#define DRAW_BUF_SIZE (SCREEN_WIDTH * DRAW_BUF_LINES * 2)

extern void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);
extern void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
extern void lcd_setup();
extern void update_UI();