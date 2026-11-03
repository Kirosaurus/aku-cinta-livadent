#pragma once

#include <lvgl.h>
#include <LittleFS.h>

#include "firebase_livadent.h"
#include "wifi_livadent.h"
#include "hardware_workflow.h"
#include "global.h"

extern void wifi_settings_btn(lv_event_t * e);
extern void list_btn_wifi_network(lv_event_t * e);
extern void wifi_scan_timer(lv_timer_t * timer);
extern void main_menu_loaded(lv_event_t * e);
extern void wifi_settings_loaded(lv_event_t * e);
extern void disconnect_btn_interface_update();
extern void disconnect_btn(lv_event_t * e);
extern void wifi_settings_heading_interface_update();
extern void splash_screen_loaded(lv_event_t* e);
extern void wifi_cancel_btn(lv_event_t* e);
extern void mulai_btn(lv_event_t* e);
extern void screening_loaded(lv_event_t * e);
extern void start_stop_screening(lv_event_t * e);
extern void hide_password_btn(lv_event_t* e);
extern void result_loaded(lv_event_t * e);
extern void done_result_btn(lv_event_t* e);
extern void no_screening_btn(lv_event_t* e);
extern void yes_screening_btn(lv_event_t* e);
extern void back_btn(lv_event_t* e);
extern void back_btn_failed(lv_event_t* e);
extern void cancel_offline_screening(lv_event_t* e);

extern void wifi_page_prev_btn(lv_event_t* e);
extern void wifi_page_next_btn(lv_event_t* e);
// extern void back_wifi_btn(lv_event_t * e);
// extern void back_btn_screening(lv_event_t * e);

extern void go_to_calibration_page(lv_event_t* e);
extern void go_to_calibration_ph_page(lv_event_t* e);
extern void go_to_calibration_visc_page(lv_event_t* e);