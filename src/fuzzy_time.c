#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 86
#define BUFFER_SIZE_BOLD 20

//#define DEBUG

static struct CommonWordsData {
  TextLayer *label;
  TextLayer *label_bold;
  Window *window;
  char buffer[BUFFER_SIZE];
  char buffer_bold[BUFFER_SIZE_BOLD];
} s_data;

static void set_layer_sizes(int rows) {
  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  if(rows == 1) {
    layer_set_hidden((Layer *)s_data.label, true);
    layer_set_bounds((Layer *)s_data.label_bold,
      GRect(0, -2, frame.size.w, 140));
  }
  else if(rows == 2) {
    layer_set_hidden((Layer *)s_data.label, false);
    layer_set_bounds((Layer *)s_data.label,
      GRect(0, 20, frame.size.w, (frame.size.h / 2) ));
    layer_set_bounds((Layer *)s_data.label_bold,
      GRect(0, 10, frame.size.w, frame.size.h / 2));
  }
  else if(rows == 3) {
    layer_set_hidden((Layer *)s_data.label, false);
    layer_set_bounds((Layer *)s_data.label,
      GRect(0, 10, frame.size.w, (160) ));
    layer_set_bounds((Layer *)s_data.label_bold,
      GRect(0, 20, frame.size.w, 300));
  }
  else {
    layer_set_hidden((Layer *)s_data.label, false);
    layer_set_bounds((Layer *)s_data.label,
      GRect(0, 0, frame.size.w, (140) ));
    layer_set_bounds((Layer *)s_data.label_bold,
      GRect(0, 30, frame.size.w, 300));
  }
}

static void update_time(struct tm* t) {
  int rows;
#ifdef DEBUG
  rows = fuzzy_time_to_words(0, 0, s_data.buffer, BUFFER_SIZE,
	s_data.buffer_bold, BUFFER_SIZE_BOLD);
#else
  rows = fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.buffer, BUFFER_SIZE,
	s_data.buffer_bold, BUFFER_SIZE_BOLD);
#endif

  set_layer_sizes(rows);

  text_layer_set_text(s_data.label, s_data.buffer);
  text_layer_set_text(s_data.label_bold, s_data.buffer_bold);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, GColorBlack);
  //GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_28);
  GFont font = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  GFont font_bold = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);

  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  s_data.label = text_layer_create(GRect(0, -10, frame.size.w, (frame.size.h /* 2*/) ));
  text_layer_set_background_color(s_data.label, GColorBlack);
  text_layer_set_text_color(s_data.label, GColorWhite);
  text_layer_set_font(s_data.label, font);
  text_layer_set_text_alignment(s_data.label, GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(s_data.label));

  s_data.label_bold = text_layer_create(GRect(0, (frame.size.h / 2) - 25, frame.size.w, (frame.size.h / 2) + 25));
  text_layer_set_background_color(s_data.label_bold, GColorBlack);
  text_layer_set_text_color(s_data.label_bold, GColorWhite);
  text_layer_set_font(s_data.label_bold, font_bold);
  text_layer_set_text_alignment(s_data.label_bold, GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(s_data.label_bold));

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void do_deinit(void) {
  window_destroy(s_data.window);
  text_layer_destroy(s_data.label);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
