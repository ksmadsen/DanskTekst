// Used https://github.com/wearewip/PebbleTextWatch as skeleton for this version of a Danish Text Watch
// Parts of the font code inspired by http://forums.getpebble.com/discussion/4265/swedish-fuzzy-time

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "DanskTekst.h"
#include "text.h"

PBL_APP_INFO(MY_UUID,
             MY_TITLE, 
             "Tanis",
             1, 2,
             DEFAULT_MENU_ICON,
#ifdef DEBUG
             APP_INFO_STANDARD_APP
#else
			 APP_INFO_WATCH_FACE
#endif
);

Window window;

typedef struct 
{
	TextLayer currentLayer;
	TextLayer nextLayer;	
	PropertyAnimation currentAnimation;
	PropertyAnimation nextAnimation;
} Line;

PblTm t;

Line line[4];
static char lineStr[4][2][BUFFER_SIZE];

static bool textInitialized = false;

GFont fontLight,fontBold;

/*********************************** ANIMATION ******************************************/

void animationStoppedHandler(struct Animation *animation, bool finished, void *context)
{
	TextLayer *current = (TextLayer *)context;
	GRect rect = layer_get_frame(&current->layer);
	rect.origin.x = 144;
	layer_set_frame(&current->layer, rect);
}

void makeAnimationsForLayers(Line *line, TextLayer *current, TextLayer *next)
{
	GRect rect = layer_get_frame(&next->layer);
	rect.origin.x -= 144;
	
	property_animation_init_layer_frame(&line->nextAnimation, &next->layer, NULL, &rect);
	animation_set_duration(&line->nextAnimation.animation, 400);
	animation_set_curve(&line->nextAnimation.animation, AnimationCurveEaseOut);
	animation_schedule(&line->nextAnimation.animation);
	
	GRect rect2 = layer_get_frame(&current->layer);
	rect2.origin.x -= 144;
	
	property_animation_init_layer_frame(&line->currentAnimation, &current->layer, NULL, &rect2);
	animation_set_duration(&line->currentAnimation.animation, 400);
	animation_set_curve(&line->currentAnimation.animation, AnimationCurveEaseOut);
	
	animation_set_handlers(&line->currentAnimation.animation, (AnimationHandlers) {
		.stopped = (AnimationStoppedHandler)animationStoppedHandler
	}, current);
	
	animation_schedule(&line->currentAnimation.animation);
}

void updateLineTo(Line *line, char lineStr[2][BUFFER_SIZE], char *value)
{
	TextLayer *next, *current;
	
	GRect rect = layer_get_frame(&line->currentLayer.layer);
	current = (rect.origin.x == 0) ? &line->currentLayer : &line->nextLayer;
	next = (current == &line->currentLayer) ? &line->nextLayer : &line->currentLayer;
	
	// Update correct text only
	if (current == &line->currentLayer) 
	{
		memset(lineStr[1], 0, BUFFER_SIZE);
		memcpy(lineStr[1], value, strlen(value));
		text_layer_set_text(next, lineStr[1]);
	} 
	else 
	{
		memset(lineStr[0], 0, BUFFER_SIZE);
		memcpy(lineStr[0], value, strlen(value));
		text_layer_set_text(next, lineStr[0]);
	}
	
	makeAnimationsForLayers(line, current, next);
}

bool needToUpdateLine(Line *line, char lineStr[2][BUFFER_SIZE], char *nextValue)
{
	char *currentStr;
	GRect rect = layer_get_frame(&line->currentLayer.layer);
	currentStr = (rect.origin.x == 0) ? lineStr[0] : lineStr[1];

	if (memcmp(currentStr, nextValue, MAX(strlen(currentStr),strlen(nextValue))) != 0 || (strlen(nextValue) == 0 && strlen(currentStr) != 0)) 
		return true;
	return false;
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

// Update screen based on new time
void display_time(PblTm *t)
{
	// The current time text will be stored in the following 4 strings
	char textLine[4][BUFFER_SIZE];
	
	int boldIndex = time_to_4words(t->tm_hour, t->tm_min, textLine[0], textLine[1], textLine[2], textLine[3], BUFFER_SIZE);
	
	for(int i = 0; i < 4; i++)
	{
		if(boldIndex == i)
			configureBoldLayer(&line[i].nextLayer);
		else
			configureLightLayer(&line[i].nextLayer);
		if (needToUpdateLine(&line[i], lineStr[i], textLine[i])) 
			updateLineTo(&line[i], lineStr[i], textLine[i]);	
	}
}

// Update screen without animation first time we start the watchface
void display_initial_time(PblTm *t)
{
	int boldIndex = time_to_4words(t->tm_hour, t->tm_min, lineStr[0][0], lineStr[1][0], lineStr[2][0], lineStr[3][0], BUFFER_SIZE);
	
	for(int i=0; i<4; i++)
	{
		if(boldIndex == i)
			configureBoldLayer(&line[i].currentLayer);
		else
			configureLightLayer(&line[i].currentLayer);
		text_layer_set_text(&line[i].currentLayer, lineStr[i][0]);
	}
}

/** 
 * Debug methods. For quickly debugging enable debug macro on top to transform the watchface into
 * a standard app and you will be able to change the time with the up and down buttons
 */ 
#ifdef DEBUG

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) 
{
	(void)recognizer;
	(void)window;
	
	t.tm_min++;
	if (t.tm_min >= 60) 
	{
		t.tm_min = 0;
		t.tm_hour += 1;
		
		if (t.tm_hour >= 24) 
		{
			t.tm_hour = 0;
		}
	}
	display_time(&t);
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) 
{
	(void)recognizer;
	(void)window;
	
	t.tm_min--;
	if (t.tm_min < 0) 
	{
		t.tm_min = 59;
		t.tm_hour -= 1;
	}
	display_time(&t);
}

void click_config_provider(ClickConfig **config, Window *window) 
{
  (void)window;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

#endif

void handle_init(AppContextRef ctx) 
{
  	(void)ctx;

	window_init(&window, MY_TITLE);
	window_stack_push(&window, true);
	window_set_background_color(&window, GColorBlack);

	// Init resources
	resource_init_current_app(&APP_RESOURCES);
	
	fontLight = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_LIGHT_32));
	fontBold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_BOLD_32));

	for(int i = 0; i < 4; i++)
	{
		text_layer_init(&line[i].currentLayer, GRect(0, 2 + (i * 37), 144, 50));
		text_layer_init(&line[i].nextLayer, GRect(144, 2 + (i * 37), 144, 50));
	}

	// Configure time on init
	get_time(&t);
	display_initial_time(&t);
	
	// Load layers
  	layer_add_child(&window.layer, &line[0].currentLayer.layer);
	layer_add_child(&window.layer, &line[0].nextLayer.layer);
	layer_add_child(&window.layer, &line[1].currentLayer.layer);
	layer_add_child(&window.layer, &line[1].nextLayer.layer);
	layer_add_child(&window.layer, &line[2].currentLayer.layer);
	layer_add_child(&window.layer, &line[2].nextLayer.layer);
	layer_add_child(&window.layer, &line[3].currentLayer.layer);
	layer_add_child(&window.layer, &line[3].nextLayer.layer);
	
#ifdef DEBUG
	// Button functionality
	window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
#endif
}

// Time handler called every minute by the system
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) 
{
  (void)ctx;

  display_time(t->tick_time);
}

void pbl_main(void *params) 
{
  PebbleAppHandlers handlers = 
  {
    .init_handler = &handle_init,
	.tick_info = 
	{
		      .tick_handler = &handle_minute_tick,
		      .tick_units = MINUTE_UNIT
		    }
  };
  app_event_loop(params, &handlers);
}