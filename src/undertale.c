#include <pebble.h>

// Number of times we animate per min
const uint32_t ANIMATION_COUNT = 3;

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;

static GBitmap *s_background_bitmap;
static BitmapLayer *s_background_layer;

static GBitmap *s_animation_bitmap;
static BitmapLayer *s_animation_layer;
static GBitmapSequence *s_sequence;

static int s_battery_level;

// Animation
static void timer_handler(void *context) {
  uint32_t next_delay;

  // Advance to the next APNG frame
  if(gbitmap_sequence_update_bitmap_next_frame(s_sequence, s_animation_bitmap, &next_delay)) {
    bitmap_layer_set_bitmap(s_animation_layer, s_animation_bitmap);
    layer_mark_dirty(bitmap_layer_get_layer(s_animation_layer));

    // Timer for that delay
    app_timer_register(next_delay, timer_handler, NULL);
  } else {
    // Start again
    gbitmap_sequence_restart(s_sequence);
  }
}

static void load_sequence() {
  if(!s_sequence){
    s_sequence = gbitmap_sequence_create_with_resource(RESOURCE_ID_ANIMATION_IMAGE);
  }

  if(!s_animation_bitmap){
    s_animation_bitmap = gbitmap_create_blank(gbitmap_sequence_get_bitmap_size(s_sequence), GBitmapFormat8Bit);
  }
  gbitmap_sequence_set_play_count(s_sequence, ANIMATION_COUNT);

  app_timer_register(1, timer_handler, NULL);
}

static void run_animatiom(){
}



// Time
static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[10];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?  "%H:%M %p" : "%I:%M %p", tick_time);

  load_sequence();
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


// Main window load/unload
static void main_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Background
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_IMAGE);
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Animation
  s_animation_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ANIMATION_IMAGE);
  s_animation_layer = bitmap_layer_create(GRect(52, 15, 40, 80));
  bitmap_layer_set_bitmap(s_animation_layer, s_animation_bitmap);
  bitmap_layer_set_compositing_mode(s_animation_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_animation_layer));

  // Time
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_DTM_MONO_14));
  s_time_layer = text_layer_create(GRect(60, 113, bounds.size.w, 20));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window){
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_time_font);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_animation_layer);
  bitmap_layer_destroy(s_background_layer);
}



// Initialize/tear down
static void init(){
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  window_stack_push(s_main_window, true);
  update_time();
}

static void deinit(){
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
