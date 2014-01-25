// Used https://github.com/wearewip/PebbleTextWatch as skeleton for this version of a Danish Text Watch
// Parts of the font code inspired by http://forums.getpebble.com/discussion/4265/swedish-fuzzy-time

#include "pebble.h"

#include "DanskTekst.h"
#include "text.h"

Window *window;

typedef struct 
{
	TextLayer *currentLayer;
	TextLayer *nextLayer;	
	PropertyAnimation *currentAnimation;
	PropertyAnimation *nextAnimation;
	int bold;
} Line;

#ifdef SHOW_DATE
TextLayer *date;
TextLayer *day;
#endif

#ifdef BATTERY_STAT
Layer *battery_layer;
BatteryChargeState battery_state;
#endif

Line line[4];
static char lineStr[4][2][BUFFER_SIZE];

GFont fontLight,fontBold;

#ifdef SHOW_DATE
void setDate(struct tm *tm)
{
  static char dateString[BUFFER_SIZE];
  static char dayString[BUFFER_SIZE];
	
  get_date_string(tm->tm_year, tm->tm_mon, tm->tm_mday, dateString, BUFFER_SIZE);
  get_weekday_string(tm->tm_wday, dayString, BUFFER_SIZE);
	
  text_layer_set_text(date, dateString);
  text_layer_set_text(day, dayString);
}
#endif 

struct tm *tm;

/*********************************** ANIMATION ******************************************/

void destroy_property_animation(PropertyAnimation **prop_animation) {
  if (*prop_animation == NULL) {
    return;
  }

  if (animation_is_scheduled((Animation*) *prop_animation)) {
    animation_unschedule((Animation*) *prop_animation);
  }

  property_animation_destroy(*prop_animation);
  *prop_animation = NULL;
}


void animationStoppedHandler(Animation *animation, bool finished, void *context)
{
  TextLayer *current = (TextLayer *)context;
  GRect rect = layer_get_frame(text_layer_get_layer(current));
  rect.origin.x = 144;
  layer_set_frame(text_layer_get_layer(current), rect);
}

void makeAnimationsForLayers(Line *line, TextLayer *current, TextLayer *next)
{
  GRect rect = layer_get_frame(text_layer_get_layer(next));
  rect.origin.x -= 144;

  destroy_property_animation(&line->nextAnimation);
  destroy_property_animation(&line->currentAnimation);
	
  line->nextAnimation = property_animation_create_layer_frame(text_layer_get_layer(next), NULL, &rect);
  animation_set_duration((Animation*) line->nextAnimation, 400);
  animation_set_curve((Animation*) line->nextAnimation, AnimationCurveEaseOut);
  animation_schedule((Animation*) line->nextAnimation);
	
  GRect rect2 = layer_get_frame(text_layer_get_layer(current));
  rect2.origin.x -= 144;
	
  line->currentAnimation = property_animation_create_layer_frame(text_layer_get_layer(current), NULL, &rect2);
  animation_set_duration((Animation*) line->currentAnimation, 400);
  animation_set_curve((Animation*) line->currentAnimation, AnimationCurveEaseOut);
	
  animation_set_handlers((Animation*)line->currentAnimation,
			 (AnimationHandlers) {
			   .stopped = (AnimationStoppedHandler) animationStoppedHandler
			     }, current);
	
  animation_schedule((Animation*) line->currentAnimation);
}

void configureBoldLayer(TextLayer *textlayer)
{
  text_layer_set_font(textlayer, fontBold);
  text_layer_set_text_color(textlayer, GColorWhite);
  text_layer_set_background_color(textlayer, GColorClear);
  text_layer_set_text_alignment(textlayer, GTextAlignmentLeft);
}

void configureLightLayer(TextLayer *textlayer)
{
  text_layer_set_font(textlayer, fontLight);
  text_layer_set_text_color(textlayer, GColorWhite);
  text_layer_set_background_color(textlayer, GColorClear);
  text_layer_set_text_alignment(textlayer, GTextAlignmentLeft);
}

void updateLineTo(Line *line, char lineStr[2][BUFFER_SIZE], char *value, int bold)
{
  TextLayer *next, *current;
	
  GRect rect = layer_get_frame(text_layer_get_layer(line->currentLayer));
  current = (rect.origin.x == 0) ? line->currentLayer : line->nextLayer;
  next = (current == line->currentLayer) ? line->nextLayer : line->currentLayer;
	
  // Update correct text only
  if (current == line->currentLayer) {
    memset(lineStr[1], 0, BUFFER_SIZE);
    memcpy(lineStr[1], value, strlen(value));
    text_layer_set_text(next, lineStr[1]);
  } else {
    memset(lineStr[0], 0, BUFFER_SIZE);
    memcpy(lineStr[0], value, strlen(value));
    text_layer_set_text(next, lineStr[0]);
  }

  if(bold) {
    configureBoldLayer(next);
  } else {
    configureLightLayer(next);
  }

  makeAnimationsForLayers(line, current, next);
}

bool needToUpdateLine(Line *line, char lineStr[2][BUFFER_SIZE], char *nextValue, int bold)
{
  char *currentStr;
  GRect rect = layer_get_frame(text_layer_get_layer(line->currentLayer));
  currentStr = (rect.origin.x == 0) ? lineStr[0] : lineStr[1];
  int oldBold = line->bold;
  line->bold = bold;

  if (oldBold != bold) {
    return true;
  }

  if (memcmp(currentStr, nextValue, MAX(strlen(currentStr),strlen(nextValue))) != 0
      || (strlen(nextValue) == 0 && strlen(currentStr) != 0)) {
    return true;
  }

  return false;
}

// Update screen based on new time
void display_time(struct tm *t)
{
  // The current time text will be stored in the following 4 strings
  char textLine[4][BUFFER_SIZE];
	
  int boldIndex = time_to_4words(t->tm_hour, t->tm_min, textLine[0], textLine[1], textLine[2], textLine[3], BUFFER_SIZE);
	
  for (int i = 0; i < 4; i++) {
    int bold = boldIndex == i;
    if (needToUpdateLine(&line[i], lineStr[i], textLine[i], bold)) 
      updateLineTo(&line[i], lineStr[i], textLine[i], bold);	
  }
}

// Update screen without animation first time we start the watchface
void display_initial_time(struct tm *t)
{
  int boldIndex = time_to_4words(t->tm_hour, t->tm_min, lineStr[0][0], lineStr[1][0],
				 lineStr[2][0], lineStr[3][0], BUFFER_SIZE);
	
  for (int i=0; i<4; i++) {
    if (boldIndex == i) {
      configureBoldLayer(line[i].currentLayer);
    } else {
      configureLightLayer(line[i].currentLayer);
    }
    text_layer_set_text(line[i].currentLayer, lineStr[i][0]);
  }

#ifdef SHOW_DATE
  setDate(t);
#endif

#ifdef BATTERY_STAT
  layer_mark_dirty(battery_layer);
#endif
}

/** 
 * Debug methods. For quickly debugging enable debug macro on top to transform the watchface into
 * a standard app and you will be able to change the time with the up and down buttons
 */ 
#ifdef DEBUG

void up_single_click_handler(ClickRecognizerRef recognizer, void *context) 
{
  tm->tm_min++;
  if (tm->tm_min >= 60) {
    tm->tm_min = 0;
    tm->tm_hour += 1;
		
    if (tm->tm_hour >= 24) {
      tm->tm_hour = 0;
    }
  }
  display_time(tm);
}


void down_single_click_handler(ClickRecognizerRef recognizer, void *context) 
{
  tm->tm_min--;
  if (tm->tm_min < 0) {
    tm->tm_min = 59;
    tm->tm_hour -= 1;
  }
  display_time(tm);
}

void click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}

#endif

#ifdef BATTERY_STAT
void battery_layer_update_proc(struct Layer *layer, GContext *ctx)
{
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  if (!battery_state.is_charging && !battery_state.is_plugged) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    GRect layer_rect = layer_get_bounds(layer);

    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, GRect(layer_rect.origin.x, 
				  layer_rect.origin.y + 3,
				  2,
				  layer_rect.size.h - 6), 0, GCornerNone);

    layer_rect.origin.x += 2;
    layer_rect.size.w -= 2;

    graphics_draw_rect(ctx, layer_rect);

    layer_rect.origin.x += 2;
    layer_rect.origin.y += 2;
    layer_rect.size.w -= 4;
    layer_rect.size.h -= 4;

    int size = layer_rect.size.w - (battery_state.charge_percent * layer_rect.size.w + 1) / 100;
    layer_rect.origin.x += size;
    layer_rect.size.w -= size;

    app_log(APP_LOG_LEVEL_INFO, __FILE__, __LINE__, "Battery stat: %d%%, size: %d", battery_state.charge_percent, size);

    graphics_fill_rect(ctx, layer_rect, 0, GCornerNone);
  }
}

void battery_state_handler(BatteryChargeState new_state)
{
  battery_state = new_state;
  layer_mark_dirty(battery_layer);
}
#endif

// Time handler called every minute by the system
void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) 
{
  display_time(tick_time);
#ifdef SHOW_DATE
  if (units_changed & DAY_UNIT) {
    setDate(tick_time);
  }
#endif
}

void handle_init() 
{
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  fontLight = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_LIGHT_32));
  fontBold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_BOLD_32));

  for (int i = 0; i < 4; i++) {
    line[i].currentLayer = text_layer_create(GRect(0, 2 + (i * 37), 144, 50));
    line[i].nextLayer = text_layer_create(GRect(144, 2 + (i * 37), 144, 50));
    line[i].currentAnimation = NULL;
    line[i].nextAnimation = NULL;
  }

#ifdef BATTERY_STAT
  battery_layer = layer_create(GRect(120, 2, 22, 10));
  layer_set_update_proc(battery_layer, battery_layer_update_proc);
#endif

#ifdef SHOW_DATE
  //date & day layers
  date = text_layer_create(GRect(0, 150, 144, 168-150));
  text_layer_set_text_color(date, GColorWhite);
  text_layer_set_background_color(date, GColorClear);
  text_layer_set_font(date, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(date, GTextAlignmentRight);

  day = text_layer_create(GRect(0, 135, 144, 168-135));
  text_layer_set_text_color(day, GColorWhite);
  text_layer_set_background_color(day, GColorClear);
  text_layer_set_font(day, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(day, GTextAlignmentRight);
#endif

  // Configure time on init
  time_t t;
  time(&t);

  tm = localtime(&t);
  display_initial_time(tm);
	
  // Load layers
  Layer *root;
  root = window_get_root_layer(window);

  for (int i = 0; i < 4; i++) {
    layer_add_child(root, text_layer_get_layer(line[i].currentLayer));
    layer_add_child(root, text_layer_get_layer(line[i].nextLayer));
  }
#ifdef SHOW_DATE
  layer_add_child(root, text_layer_get_layer(date));
  layer_add_child(root, text_layer_get_layer(day));
#endif

#ifdef BATTERY_STAT
  battery_state = battery_state_service_peek();
  layer_add_child(root, battery_layer);
  battery_state_service_subscribe(battery_state_handler);
#endif

#ifdef DEBUG
  // Button functionality
  window_set_click_config_provider(window, click_config_provider);
#endif
}

void handle_deinit()
{
  tick_timer_service_unsubscribe();
  window_stack_remove(window, false);
  window_destroy(window);

#ifdef SHOW_DATE
  text_layer_destroy(day);
  text_layer_destroy(date);
#endif

#ifdef BATTERY_STAT
  battery_state_service_unsubscribe();
  layer_destroy(battery_layer);
#endif

  for (int i = 0; i < 4; i++) {
    text_layer_destroy(line[i].nextLayer);
    text_layer_destroy(line[i].currentLayer);
    destroy_property_animation(&line[i].nextAnimation);
    destroy_property_animation(&line[i].currentAnimation);
  }

  fonts_unload_custom_font(fontLight);
  fonts_unload_custom_font(fontBold);
}

int main(void) 
{
  handle_init();
  app_event_loop();
  handle_deinit();

  return 0;
}
