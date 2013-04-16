// Used https://github.com/wearewip/PebbleTextWatch as skeleton for this version of a Danish Text Watch
// Parts of the font code inspired by http://forums.getpebble.com/discussion/4265/swedish-fuzzy-time

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define DEBUG 0
#define BUFFER_SIZE 44

#define MY_UUID { 0xE3, 0x8D, 0xFC, 0x3E, 0x1F, 0x96, 0x4C, 0x0F, 0xAF, 0x6F, 0x78, 0x60, 0xA5, 0x67, 0xEB, 0x27 }
PBL_APP_INFO(MY_UUID,
             "Dansk Tekst", 
             "Tanis",
             1, 0,
             DEFAULT_MENU_ICON,
#if DEBUG
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

static const char * const NUMBERS[] = 
{
    "tolv",
    "et",
    "to",
    "tre",
    "fire",
    "fem",
    "seks",
    "syv",
    "otte",
    "ni",
    "ti",
    "elleve"
};

void get_hour_string(int hour, int minute, char *hourBuffer, size_t length)
{
    memset(hourBuffer, 0, length);
    
    if (minute > 15)
        hour++;
        
    strcat(hourBuffer, NUMBERS[hour % 12]);
}

int get_minute_string(int minute, char *line1, char *line2, char *line3)
{
	int lines = 0;
    int m1 = minute % 15;
    int m2 = minute / 15;
    if(minute == 15)
    	m2 = 0;
    if(m2 == 1 || m2 == 3)
    	m1 = 15 - m1;
    
    lines++;
    switch(m1)
    {
    	case 1:
	    	strcat(line1, "et");
	    	strcat(line2, "minut");
	    	lines++;
	    	break;
	    case 2:
	    	strcat(line1, "to");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 3:
	    	strcat(line1, "tre");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 4:
	    	strcat(line1, "fire");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 5:
	    	strcat(line1, "fem");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 6:
	    	strcat(line1, "seks");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 7:
	    	strcat(line1, "syv");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 8:
	    	strcat(line1, "otte");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 9:
	    	strcat(line1, "ni");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 10:
	    	strcat(line1, "ti");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 11:
	    	strcat(line1, "elleve");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 12:
	    	strcat(line1, "tolv");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 13:
	    	strcat(line1, "tretten");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 14:
	    	strcat(line1, "fjorten");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 0:
	    case 15:
	    	strcat(line1, "kvart");
	    	break;
	}	    

	lines++;
	char *line = line2;
	if(lines == 3)
		line = line3;
    switch(m2)
    {
    	case 0:
    		strcat(line, "over");
    		break;
    	case 1:
    		strcat(line, "i halv");
    		break;
    	case 2:
    		strcat(line, "over halv");
    		break;
    	case 3:
    		strcat(line, "i");
    		break;
    }
    
    return lines;
}

int time_to_4words(int hours, int minutes, char *line1, char *line2, char *line3, char *line4, size_t length)
{
	memset(line1, 0, length);
	memset(line2, 0, length);
	memset(line3, 0, length);
	memset(line4, 0, length);

	char hourText[length];
	get_hour_string(hours, minutes, hourText, length);
	
	if(minutes == 0)
	{
		strcpy(line1, hourText);
		return 0;
	}
	else if(minutes == 30)
	{
		strcpy(line1, "halv");
		strcpy(line2, hourText);
		return 1;
	}
	else
	{
		int lineCount = get_minute_string(minutes, line1, line2, line3);
		char *line = lineCount == 2 ? line3 : line4;
		strcpy(line, hourText);
		return lineCount;
	}
}

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

	if (memcmp(currentStr, nextValue, strlen(nextValue)) != 0 || (strlen(nextValue) == 0 && strlen(currentStr) != 0)) 
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
#if DEBUG

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

	window_init(&window, "Dansk Tekst");
	window_stack_push(&window, true);
	window_set_background_color(&window, GColorBlack);

	// Init resources
	resource_init_current_app(&APP_RESOURCES);
	
	fontLight = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_LIGHT_34));
	fontBold = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_BOLD_34));

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
	
#if DEBUG
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