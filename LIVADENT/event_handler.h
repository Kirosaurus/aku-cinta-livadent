#pragma once

#include <lvgl.h>
#include "wifi_livadent.h"
#include "global.h"

extern void wifi_settings_btn(lv_event_t * e);
extern void list_btn_wifi_network(lv_event_t * e);
extern void back_wifi_btn(lv_event_t * e);
extern void wifi_scan_timer(lv_timer_t * timer);
extern void main_menu_loaded(lv_event_t * e);
extern void wifi_settings_loaded(lv_event_t * e);
extern void disconnect_btn_interface_update();
extern void disconnect_btn(lv_event_t * e);
extern void wifi_settings_heading_interface_update();
extern void splash_screen_loaded(lv_event_t* e);