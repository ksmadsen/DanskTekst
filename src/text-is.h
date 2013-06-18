#include "pebble_os.h"

extern int time_to_4words_is(int hours, int minutes, char *line1, char *line2, char *line3, char *line4, size_t length);
extern void get_date_string_is(int year, int month, int day, char *dateBuffer, size_t length);
extern void get_weekday_string_is(int daysSinceSunday, char *weekdayBuffer, size_t length);
