#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_hour_layer, *s_min_layer, *s_date_layer;
static GFont s_time_font, s_date_font;


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char hour_buffer[] = "00";
  static char min_buffer[] = "00";
  static char date_buffer[] = "00.00";

  // Write the current hours
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(hour_buffer, sizeof("00"), "%H", tick_time);
  } else {
    // Use 12 hour format
    strftime(hour_buffer, sizeof("00"), "%I", tick_time);
  }
  
  // Write the current min
  strftime(min_buffer, sizeof("00"), "%M", tick_time);
  
  // Write the current date
  strftime(date_buffer, sizeof("00.00"), "%e.%m", tick_time);

  // Display this on the TextLayer
  text_layer_set_text(s_hour_layer, hour_buffer);
  text_layer_set_text(s_min_layer, min_buffer);
  text_layer_set_text(s_date_layer, date_buffer);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_CAFETA_78));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_CAFETA_32));
  
  // Create hour TextLayer
  s_hour_layer = text_layer_create(GRect(0, 31, 70, 80));
  text_layer_set_background_color(s_hour_layer, GColorBlack);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_hour_layer, GColorElectricBlue);
#else
  text_layer_set_text_color(s_hour_layer, GColorWhite);
#endif

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_hour_layer, s_time_font);
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentRight);
  
  // Create min TextLayer
  s_min_layer = text_layer_create(GRect(74, 31, 70, 80));
  text_layer_set_background_color(s_min_layer, GColorBlack);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_min_layer, GColorShockingPink);
#else
  text_layer_set_text_color(s_min_layer, GColorWhite);
#endif

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_min_layer, s_time_font);
  text_layer_set_text_alignment(s_min_layer, GTextAlignmentLeft);
  
  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(0, 121, 144, 32));
  text_layer_set_background_color(s_date_layer, GColorBlack);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_date_layer, GColorWhite);
#else
  text_layer_set_text_color(s_date_layer, GColorWhite);
#endif
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  //layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_min_layer));
}

static void main_window_unload(Window *window) {
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  
  // Destroy TextLayer
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_min_layer);
  text_layer_destroy(s_date_layer);

}

static void init() {
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}