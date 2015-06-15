#include <pebble.h>
  
#define PERSIST_DEFAULTS_SET 3489
#define PERSIST_KEY_DATE        1
  
static Window *s_main_window;
static TextLayer *s_hour_layer, *s_min_layer, *s_date_layer;
static Layer *s_main_layer;
static GFont s_time_font, s_date_font;
static bool s_display_date;

/******************* UPDATE TIME *******************/

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

/******************* WINDOW LOAD *******************/

static void main_window_load(Window *window) {
  
  // Main layer
  s_main_layer = layer_create(GRect(0, 0, 144, 168));
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_GOTHAM_78));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_GOTHAM_32));
  
  // Create hour TextLayer
  s_hour_layer = text_layer_create(GRect(8, 33, 64, 78));
  text_layer_set_background_color(s_hour_layer, GColorBlack);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_hour_layer, GColorElectricBlue);
#else
  text_layer_set_text_color(s_hour_layer, GColorWhite);
#endif

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_hour_layer, s_time_font);
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);
  
  // Create min TextLayer
  s_min_layer = text_layer_create(GRect(72, 33, 72, 78));
  text_layer_set_background_color(s_min_layer, GColorBlack);
#ifdef PBL_COLOR
  text_layer_set_text_color(s_min_layer, GColorShockingPink);
#else
  text_layer_set_text_color(s_min_layer, GColorWhite);
#endif

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_min_layer, s_time_font);
  text_layer_set_text_alignment(s_min_layer, GTextAlignmentCenter);
  
  
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
  if (s_display_date) {
    layer_add_child(s_main_layer, text_layer_get_layer(s_date_layer));
  }
  layer_add_child(s_main_layer, text_layer_get_layer(s_hour_layer));
  layer_add_child(s_main_layer, text_layer_get_layer(s_min_layer));
  
  // Add Main layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), s_main_layer);
}

/******************* WINDOW UNLOAD *******************/

static void main_window_unload(Window *window) {
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  
  // Destroy TextLayer
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_min_layer);
  text_layer_destroy(s_date_layer);

}

/******************* INIT *******************/

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

/******************* DEINIT *******************/

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

/******************* LOAD CONFIG *******************/

void config_init() {
  // Set defaults
  if(!persist_exists(PERSIST_DEFAULTS_SET)) {
    persist_write_bool(PERSIST_DEFAULTS_SET, true);
    persist_write_bool(PERSIST_KEY_DATE, true);
  }

  s_display_date = persist_read_bool(PERSIST_KEY_DATE);
  
  // if the s_date_layer is not null
  if (s_date_layer) {
    // if we don't want to display
    if (!s_display_date) {
      // hide it
      layer_set_hidden(text_layer_get_layer(s_date_layer), true);
    }
    else {
      // unhide it
      layer_set_hidden(text_layer_get_layer(s_date_layer), false);
    }
  }
  
}

/******************* COMMUNICATION *******************/

static void in_recv_handler(DictionaryIterator *iter, void *context) {
  Tuple *t = dict_read_first(iter);
 
  // Process all pairs present
  while(t != NULL) {
    // Process this pair's key
    switch (t->key) {
      case PERSIST_KEY_DATE:
        persist_write_bool(PERSIST_KEY_DATE, strcmp(t->value->cstring, "true") == 0 ? true : false);
        break;
    }

    // Get next pair, if any
    t = dict_read_next(iter);
  }
      
  // Refresh live store
  config_init();
  vibes_short_pulse();
}

void comm_init() {
  app_message_register_inbox_received(in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

/******************* MAIN *******************/

int main(void) {
  comm_init();
  config_init();
  init();
  app_event_loop();
  deinit();
}