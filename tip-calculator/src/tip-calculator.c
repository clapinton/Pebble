#include "pebble.h"

static Window *windowSet;

static TextLayer *textDollar;
static TextLayer *textCent;
static TextLayer *textValues;

static GBitmap *action_icon_plus;
static GBitmap *action_icon_minus;

static ActionBarLayer *action_bar;

int dollars, cents, editValue = 0;
static int editDollar = 1;

static char strValue[200]; /* This will receive the final text line, to be displayed on the screen. */


static void update_text() {
/* I need to use snprintf. It takes a buffer where the resulting C string is stored, the size of that buffer and the string itself.
Use %u (unsigned integer) for variables. Then "," and place the variables that will replace the %u, in the order they were placed. */

static char text_buffer_dollar [50];
static char text_buffer_cent [50];

snprintf(text_buffer_dollar, sizeof(text_buffer_dollar), "$%u", dollars);
snprintf(text_buffer_cent, sizeof(text_buffer_cent), "¢%u", cents);
text_layer_set_text(textDollar, text_buffer_dollar);
text_layer_set_text(textCent, text_buffer_cent);
}

static void calculateValues() {

	int billValue;
	int tipValue[5];
	int totalValue[5];	
	char strLine[10] = "$";	


	billValue = 100 * dollars + cents;
	
	for (int i=0; i<=5; i++) {
		tipValue[i] = (15+i) * billValue;
		totalValue[i] = billValue * (115+i);
/*	  snprintf(strLine, sizeof(strLine), "$%u,%u + %u", dollars, cents, 10+i, tipValue[i], totalValue[i]);	*/
	  snprintf(strLine, sizeof(strLine), "%u,%u", dollars, cents);		   /* Prepare bill value. */
		strcat(strValue, strLine);
		strcat(strValue, " + "); 
	  snprintf(strLine, sizeof(strLine), "%u", 15+i);		  /* Prepare tip amount. */
		strcat(strValue, strLine);
		strcat(strValue, "%: $");		
	  snprintf(strLine, sizeof(strLine), "%u", tipValue[i]);		  /* Prepare tip value. */
		strcat(strValue, strLine);
		strcat(strValue, " , $");
	  snprintf(strLine, sizeof(strLine), "%u\n", totalValue[i]);		  /* Prepare total value. */
		strcat(strValue, strLine);
		
}		

}


static void displayResults() {

  Layer *window_layer = window_get_root_layer(windowSet);
  const int16_t width = layer_get_frame(window_layer).size.w - ACTION_BAR_WIDTH - 3;	
	

  /* Destroy the dollar and cent layers to make room for the results.*/
  text_layer_destroy(textDollar);
  text_layer_destroy(textCent);
  
  
	/* Setup Text Layer for table. */
  textValues = text_layer_create(GRect(0, 5, width, 140));		/* Allocate memory for a text layer. Must be destroyed afterwards. */
  text_layer_set_font(textValues, fonts_get_system_font(FONT_KEY_GOTHIC_14));		/*Set the font of the string.*/
  text_layer_set_text_alignment(textValues, GTextAlignmentRight);		/* GTextAlignment aligns the layer on the first parameter, regarding the edges of GRect. */
  layer_add_child(window_layer, text_layer_get_layer(textValues));		/* Second parameter layer becomes child of the first parameter layer. Will remove any prior inheriting. */


/* @@@@@ Build a for loop to create the rows for the table. Use \n to break lines. @@@@@ */
	
	calculateValues();
	
	text_layer_set_text(textValues, strValue);
  
}



static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (editDollar == 1) {
		dollars += 2;
	} else {
		cents += 2;
	}	
  update_text();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	
	if (editDollar == 1) {
		editDollar = 0;
		action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, action_icon_plus);
	} else {
		displayResults();
	}
	
	
	
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (editDollar == 1) {
		dollars--;
	} else {
		cents--;
	}	
  update_text();
}

/*Click config for windowSet.*/
static void windowSet_click_config_provider(void *context) {
  const uint16_t repeat_interval_ms = 50;
  
	if (editDollar == 1) {
		editValue = dollars; 
	} else {
		editValue = cents;	
	}	

  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, down_click_handler);
}

/*Loading and Unloading of windowSet.*/
static void windowSet_load(Window *me) {

/*Set up right action bar.*/
  action_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar, me);
  action_bar_layer_set_click_config_provider(action_bar, windowSet_click_config_provider);

/* Place icons on the Action Bar. */
  action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, action_icon_plus);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, action_icon_minus);
  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, action_icon_minus);

  Layer *window_layer = window_get_root_layer(me);
  const int16_t width = layer_get_frame(window_layer).size.w - ACTION_BAR_WIDTH - 3;

  
	/* Setup Text for Dollar value */
  textDollar = text_layer_create(GRect(0, 25, width, 60));		/* Allocate memory for a text layer. Must be destroyed afterwards. */
  text_layer_set_font(textDollar, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));		/*Set the font of the string.*/
  text_layer_set_text_alignment(textDollar, GTextAlignmentRight);		/* GTextAlignment aligns the layer on the first parameter, regarding the edges of GRect. */
  layer_add_child(window_layer, text_layer_get_layer(textDollar));		/* Second parameter layer becomes child of the first parameter layer. Will remove any prior inheriting. */
	text_layer_set_text(textDollar, "$00");

	/* Setup Text for Cent value */
  textCent = text_layer_create(GRect(0, 75, width, 60));		/*Allocate memory for a text layer. Must be destroyed afterwards. */
  text_layer_set_font(textCent, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));		/*Set the font of the string.*/
  text_layer_set_text_alignment(textCent, GTextAlignmentRight);		/* GTextAlignment aligns the layer on the first parameter, regarding the edges of GRect. */
  layer_add_child(window_layer, text_layer_get_layer(textCent));		/* Second parameter layer becomes child of the first parameter layer. Will remove any prior inheriting. */
	text_layer_set_text(textCent, "¢00");

}

static void windowSet_unload(Window *window) {
  text_layer_destroy(textDollar);
  text_layer_destroy(textCent);

  action_bar_layer_destroy(action_bar);
}


static void init() {

  action_icon_plus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_PLUS);
  action_icon_minus = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ACTION_ICON_MINUS);

  windowSet = window_create();		/*Remember to destroy every window that you create.*/
  window_set_window_handlers(windowSet, (WindowHandlers) {
    .load = windowSet_load,
    .unload = windowSet_unload,
  });  
  
  window_set_click_config_provider(windowSet, windowSet_click_config_provider);  
  window_stack_push(windowSet, true /* Animated */);		/* Push window to screen. Set 'true' for Animation. */
    
  
}

static void deinit() {
  window_destroy(windowSet);		/* Dumps the parameter Window. Can be called while shown. All elements on it must be dumped before. */

  gbitmap_destroy(action_icon_plus);
  gbitmap_destroy(action_icon_minus);  
}

int main(void) {
  init();		/*Good practice to start a main with init*/
  app_event_loop();		/*Main loop. Won't get to next line until app exists. */
  deinit();		/*Deinitialization of the app.*/
}
