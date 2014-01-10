#include "pebble.h"

#include "DanskTekst.h"

#if LANG == LANG_DANISH
#include "text-da.h"
#elif LANG == LANG_ICELANDIC
#include "text-is.h"
#endif

int time_to_4words(int hours, int minutes, char *line1, char *line2, char *line3, char *line4, size_t length)
{
#if LANG == LANG_DANISH
	return time_to_4words_da(hours, minutes, line1, line2, line3, line4, length);
#elif LANG == LANG_ICELANDIC
	return time_to_4words_is(hours, minutes, line1, line2, line3, line4, length);
#endif
}

void get_date_string(int year, int month, int day, char *dateBuffer, size_t length)
{
#if LANG == LANG_DANISH
	get_date_string_da(year, month, day, dateBuffer, length);
#elif LANG == LANG_ICELANDIC
	get_date_string_is(year, month, day, dateBuffer, length);
#endif
}

void get_weekday_string(int daysSinceSunday, char *weekdayBuffer, size_t length)
{
#if LANG == LANG_DANISH
	get_weekday_string_da(daysSinceSunday, weekdayBuffer, length);
#elif LANG == LANG_ICELANDIC
	get_weekday_string_is(daysSinceSunday, weekdayBuffer, length);
#endif
}
