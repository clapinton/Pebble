#include "pebble.h"

static Window *windowSet;

static TextLayer *textDollar;
static TextLayer *textCent;
static TextLayer *textValues;

static GBitmap *action_icon_plus;
static GBitmap *action_icon_minus;

static ActionBarLayer *action_bar;

int dollars, cents, editValue = 0;		/* Dollars and Cents take the value of the bill, entered on the first screen. */
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
	int valueLen[3];		/* This will store the length of the value numbers, so we'll know where exactly to place the "," */	
											/* 0 is for billValue; 1 is for tipValue; 2 is for totalValue. */
	char strLine[10];
	char *strBuffer = "0";


	billValue = 100 * dollars + cents;
/*	if (billValue >= 10000) {valueLen[0] = 5;}
	else if (billValue >= 1000) {valueLen[0] = 4;}
	else if (billValue >= 100) {valueLen[0] = 3;}		
	else if (billValue >= 10) {valueLen[0] = 2;}
	else if (billValue >= 1) {valueLen[0] = 1;}	
*/	
	
	for (int i=0; i<=5; i++) {
		tipValue[i] = (15+i) * billValue;

		
		totalValue[i] = billValue * (115+i);
		if (totalValue[i] >= 10000000) {valueLen[2] = 8;}
		else if (totalValue[i] >= 1000000) {valueLen[2] = 7;}
		else if (totalValue[i] >= 100000) {valueLen[2] = 6;}
		else if (totalValue[i] >= 10000) {valueLen[2] = 5;}				
		else if (totalValue[i] >= 1000) {valueLen[2] = 4;}
		else if (totalValue[i] >= 100) {valueLen[2] = 3;}		
		else if (totalValue[i] >= 10) {valueLen[2] = 2;}
		else {valueLen[2] = 1;}			

	
		
/*	  snprintf(strLine, sizeof(strLine), "$%u,%u + %u", dollars, cents, 10+i, tipValue[i], totalValue[i]);	*/
/***** Prepare bill value for str. *****/
	  snprintf(strLine, 7, "%u,%u", dollars, cents);
		strcat(strValue, strLine);
		strcat(strValue, " + ");
		
/***** Prepare tip percentage for str. *****/		
	  snprintf(strLine, 3, "%u", 15+i);
		strcat(strValue, strLine);
		strcat(strValue, "%: $");		

		if (tipValue[i] >= 1000000) {valueLen[1] = 7-4;}
		else if (tipValue[i] >= 100000) {valueLen[1] = 6-4;}
		else if (tipValue[i] >= 10000) {valueLen[1] = 5-4;}				
		else if (tipValue[i] >= 1000) {valueLen[1] = 4;}
		else if (tipValue[i] >= 100) {valueLen[1] = 3;}	
		else if (tipValue[i] >= 10) {valueLen[1] = 2;}
		else {valueLen[1] = 1;}	

***** The lines above have to be modified. valueLen should store the comma Pos, so we are doing -4.
***** The snippet below is to print something to the screen and check the value.
***** Remove the snprintf below and uncomment the code that follows it. That is the one that should work.

/*
		snprintf(strBuffer, valueLen[1] + 1, "%u", tipValue[i]);
		snprintf(strValue, 3, "%u", valueLen[2]);
		strcat(strBuffer, "\0 \n");		
		
		text_layer_set_text(textValues, strBuffer);		
*/	

		snprintf(strLine, valueLen[1] + 4, "%u", tipValue[i]);
	
/*		snprintf(strBuffer, valueLen[1] + 1, "%u", tipValue[i]);
	  strncpy(strLine, strBuffer, valueLen[1]);
	  strLine[valueLen[1]] = '\0';
	  strcat(strLine, ",");
	  strcat(strLine, strBuffer + valueLen[1]);
*/
		strcat(strValue, strLine);
		
		strcat(strValue, " , $");
		
	  snprintf(strLine, valueLen[2] + 1, "%u", totalValue[i]);		  /* Prepare total value. Add 1 for /0 at the end. */
		strcat(strValue, strLine);
		strValue[strlen(strValue)] = '\n';	
			
		
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
