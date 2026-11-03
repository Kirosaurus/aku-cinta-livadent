#include "esp_heap_caps.h"
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

lv_color_t *draw_buf = nullptr;

TFT_eSPI tft = TFT_eSPI();
uint16_t calData[5] = { 410, 3430, 323, 3386, 7 };

int x, y, z;
lv_obj_t *touch_debug_label;

uint32_t lastTick = 0;

void lcd_setup() {
  lv_init();

  tft.begin();
  tft.setRotation(1);
  tft.setTouch(calData);

  lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_flush_cb(disp, my_disp_flush);

  draw_buf = (lv_color_t *)heap_caps_malloc(DRAW_BUF_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

  if (draw_buf == NULL) {
    Serial.println("Gagal mengalokasikan draw buffer!");
  }

  lv_display_set_buffers(
    disp,
    draw_buf,
    NULL,
    DRAW_BUF_SIZE,
    LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  Serial.printf(
    "DMA : %d\n",
    heap_caps_check_integrity(MALLOC_CAP_DMA, true));

  Serial.printf(
    "Internal heap : %u\n",
    heap_caps_get_free_size(MALLOC_CAP_INTERNAL));

  ui_init();

  // Ngasih Event Handler
  lv_obj_add_event_cb(objects.connect_wifi_btn, WiFi_connect, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.pengaturan_wifi_btn, wifi_settings_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.back_wifi_btn, back_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.main_menu, main_menu_loaded, LV_EVENT_SCREEN_LOADED, NULL);

  lv_obj_add_event_cb(objects.wifi_settings, wifi_settings_loaded, LV_EVENT_SCREEN_LOADED, NULL);

  lv_obj_add_event_cb(objects.disconnect_wifi_btn, disconnect_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.splash_screen, splash_screen_loaded, LV_EVENT_SCREEN_LOADED, NULL);

  lv_obj_add_event_cb(objects.cancel_btn, wifi_cancel_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.back_btn_screening, back_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.mulai_btn, mulai_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.screening, screening_loaded, LV_EVENT_SCREEN_LOADED, NULL);

  lv_obj_add_event_cb(objects.start_stop_screening_btn, start_stop_screening, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.hide_password_btn, hide_password_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.result, result_loaded, LV_EVENT_SCREEN_LOADED, NULL);

  lv_obj_add_event_cb(objects.result_done_btn, done_result_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.continue_offline_screening, yes_screening_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.back_offline_screening, cancel_offline_screening, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.failed_back_to_main_menu_btn, back_btn_failed, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.wifi_page_prev_btn, wifi_page_prev_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.wifi_page_next_btn, wifi_page_next_btn, LV_EVENT_CLICKED, NULL);
  
  lv_obj_add_event_cb(objects.logo_livadent_main_menu, go_to_calibration_page, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.back_btn_menu_calibration, back_btn, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.back_btn_visc_calibration, go_to_calibration_page, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.back_btn_visc_calibration, go_to_calibration_page, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.ph_calibrate_option, go_to_calibration_ph_page, LV_EVENT_CLICKED, NULL);

  lv_obj_add_event_cb(objects.visc_calibrate_option, go_to_calibration_visc_page, LV_EVENT_CLICKED, NULL);

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

  tft.pushColors((uint16_t *)px_map, w * h, true);

  tft.endWrite();

  lv_display_flush_ready(disp);
}

void update_UI() {
  lv_tick_inc(millis() - lastTick);
  lastTick = millis();
  lv_timer_handler(); 

  ui_tick();
}
