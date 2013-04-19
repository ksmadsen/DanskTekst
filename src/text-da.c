#include "pebble_os.h"

#include "DanskTekst.h"

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

void get_hour_string_da(int hour, int minute, char *hourBuffer, size_t length)
{
    memset(hourBuffer, 0, length);
    
    if (minute >= 25)
        hour++;
        
    strcat(hourBuffer, NUMBERS[hour % 12]);
}

int get_minute_string_da(int minute, char *line1, char *line2, char *line3, size_t length)
{
#ifdef FUZZY
	minute = (((minute + 2) / 5) * 5) % 60;
#endif

    memset(line1, 0, length);
    memset(line2, 0, length);
    memset(line3, 0, length);

	int lines = 0;
    int m1;
    int m2;
    if(minute < 25)
    {
    	m1 = minute;
    	m2 = 0;
    }
    else if(minute < 30)
    {
    	m1 = 30 - minute;
    	m2 = 1;
    }
    else if(minute < 36)
    {
    	m1 = minute % 30;
    	m2 = 2;
    }
    else
    {
    	m1 = 60 - minute;
    	m2 = 3;
    }
    
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
	    case 16:
	    	strcat(line1, "seksten");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 17:
	    	strcat(line1, "sytten");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 18:
	    	strcat(line1, "atten");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 19:
	    	strcat(line1, "nitten");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 20:
	    	strcat(line1, "tyve");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 21:
	    	strcat(line1, "enogtyve");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 22:
	    	strcat(line1, "toogtyve");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 23:
	    	strcat(line1, "treogtyve");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 24:
	    	strcat(line1, "fireogtyve");
	    	strcat(line2, "minutter");
	    	lines++;
	    	break;
	    case 25:
	    	strcat(line1, "femogtyve");
	    	strcat(line2, "minutter");
	    	lines++;
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

int time_to_4words_da(int hours, int minutes, char *line1, char *line2, char *line3, char *line4, size_t length)
{
	memset(line1, 0, length);
	memset(line2, 0, length);
	memset(line3, 0, length);
	memset(line4, 0, length);

	char hourText[length];
	get_hour_string_da(hours, minutes, hourText, length);
	
	if(minutes == 0)
	{
		strcpy(line1, "klokken");
		strcpy(line2, hourText);
		return 1;
	}
	else if(minutes == 30)
	{
		strcpy(line1, "klokken");
		strcpy(line2, "halv");
		strcpy(line3, hourText);
		return 2;
	}
	else
	{
		int lineCount = get_minute_string_da(minutes, line1, line2, line3, length);
		char *line = lineCount == 2 ? line3 : line4;
		strcpy(line, hourText);
		return lineCount;
	}
}
