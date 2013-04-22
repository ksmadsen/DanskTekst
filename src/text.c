#include "pebble_os.h"

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