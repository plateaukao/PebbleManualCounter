#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static uint32_t COUNT_PKEY=1;

static int count = 0;

char *itoa(int i)
{
  static char buffer[12];

  if (snprintf(buffer, sizeof(buffer), "%d", i) < 0)
    return NULL;

  return buffer;
}

static void update_count(int count) {
  text_layer_set_text(text_layer, itoa(count));
  persist_write_int(COUNT_PKEY, count);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  count = 0;
  update_count(count);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  count+=1;
  update_count(count);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  count-=1;
  if(count <=0) count = 0;
  update_count(count);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);


  text_layer = text_layer_create((GRect) { .origin = { 0, 80 }, .size = { bounds.size.w, bounds.size.h } });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

  if(!persist_exists(COUNT_PKEY)){
    text_layer_set_text(text_layer, "Hello\nDaniel's World!\n\n\nPress Up\nor Down");
  } else {
    count = persist_read_int(COUNT_PKEY);
    update_count(count);
  }

  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
