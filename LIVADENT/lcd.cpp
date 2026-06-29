#include "wifi_livadent.h"
#include "lcd.h"

// #include "UI_LCD/actions.h"
// #include "UI_LCD/fonts.h"
// #include "UI_LCD/images.h"
// #include "UI_LCD/screens.h"
// #include "UI_LCD/structs.h"
// #include "UI_LCD/styles.h"
// #include "UI_LCD/ui.h"
// #include "UI_LCD/vars.h"
#include <UI_LCD.h>

uint32_t draw_buf[DRAW_BUF_SIZE / 4];

TFT_eSPI tft = TFT_eSPI();
uint16_t calData[5] = { 410, 3430, 323, 3386, 7 };

int x, y, z;
lv_obj_t *touch_debug_label;

uint32_t lastTick = 0;

void lcd_setup(){
    lv_init();

  tft.begin();
  tft.setRotation(1);
  tft.setTouch(calData);

  lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(
    disp,
    draw_buf,
    NULL,
    sizeof(draw_buf),
    LV_DISPLAY_RENDER_MODE_PARTIAL
  );

  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  ui_init();

  // Ngasih Event Handler
  lv_obj_add_event_cb(objects.connect_wifi_btn, WiFi_connect, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.pengaturan_wifi_btn, wifi_settings_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.back_wifi_btn, back_wifi_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.main_menu, main_menu_loaded, LV_EVENT_SCREEN_LOADED, NULL);

  lv_obj_add_event_cb(objects.wifi_settings, wifi_settings_loaded, LV_EVENT_SCREEN_LOADED, NULL);

  lv_obj_add_event_cb(objects.disconnect_wifi_btn, disconnect_btn, LV_EVENT_CLICKED, NULL);
  
  lv_obj_add_event_cb(objects.splash_screen, splash_screen_loaded, LV_EVENT_SCREEN_LOADED, NULL);

  tft.setRotation(1);
}

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  uint16_t x = 0;
  uint16_t y = 0;

  bool pressed = tft.getTouch(&x, &y);

  if (!pressed) {
    data->state = LV_INDEV_STATE_RELEASED;
    return;
  }

  data->state = LV_INDEV_STATE_PRESSED;
  data->point.x = x;
  data->point.y = y;

  static uint32_t last_print = 0;
  if (millis() - last_print > 150) {
    Serial.print("Touch X: ");
    Serial.print(x);
    Serial.print(" | Touch Y: ");
    Serial.println(y);
    last_print = millis();
  }

  if (touch_debug_label != NULL) {
    char buf[64];
    snprintf(buf, sizeof(buf), "Touch x:%d y:%d", x, y);
    lv_label_set_text(touch_debug_label, buf);
  }
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);

  // Jika warna terlihat salah, coba ubah true menjadi false.
  tft.pushColors((uint16_t *)px_map, w * h, true);

  tft.endWrite();

  lv_display_flush_ready(disp);
}

void update_UI() {
  lv_tick_inc(millis() - lastTick); //--> Update the tick timer. Tick is new for LVGL 9.
  lastTick = millis();
  lv_timer_handler(); //--> Update the UI.

  ui_tick();
}
