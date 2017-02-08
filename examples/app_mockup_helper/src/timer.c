/*

   Demonstrate how the timer feature works.

 */

#include "pebble.h"

static Window *windowMain;

static TextLayer *txtlayerCounter;
static TextLayer *txtlayerMovement;

static AppTimer *timer;

int repCounter = 0; /* Repetition counter. */
int repTimer = 0; /* Repetition timer. */
int indexMovement = 0; /* movement array index. Can be 1 (UP) or 0 (DOWN). */

char txtCounter[3]; /*Counter text after snprintf. */
char movement[2][5]; /* Array with UP and DOWN words. */

bool started = false;



static void timer_callback(void *data) {
  
  if(started) {
    repTimer++;
    if (repTimer == 7) {
      vibes_short_pulse();
      indexMovement = !indexMovement;
      repTimer = 1;
      if (indexMovement == 0) {repCounter++;}
    }
    text_layer_set_text(txtlayerMovement, movement[indexMovement]);    
    snprintf(txtCounter, sizeof(txtCounter), "%u", repCounter);  
    text_layer_set_text(txtlayerCounter, txtCounter);
    if (repCounter == 50) { /* Finished the series. */
      started = false;
      vibes_long_pulse();
    } else {
      timer = app_timer_register(1000 /* milliseconds */, timer_callback, NULL);  
    }
  }
  
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  started = !started;
  timer_callback(NULL);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}


static void init () {
  
  strcpy(movement[0], "DOWN");
  strcpy(movement[1], "UP");

  windowMain = window_create();
  window_set_click_config_provider(windowMain, click_config_provider);    
  window_stack_push(windowMain, true /* Animated */);
  Layer *window_layer = window_get_root_layer(windowMain);  
  
  
  txtlayerCounter = text_layer_create(GRect(20, 30, 100, 60));
  text_layer_set_text(txtlayerCounter, "0");
  text_layer_set_font(txtlayerCounter, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(txtlayerCounter, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(txtlayerCounter));
  
  txtlayerMovement = text_layer_create(GRect(70, 100, 100, 60));
  text_layer_set_text(txtlayerMovement, "DOWN");
  text_layer_set_font(txtlayerMovement, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(txtlayerCounter, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(txtlayerMovement));
  
}

int main(void) {

  init();
  // A timer can be canceled with `app_timer_cancel()`

  app_event_loop();

  text_layer_destroy(txtlayerCounter);
  window_destroy(windowMain);
}
