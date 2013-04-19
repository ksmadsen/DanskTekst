#include "pebble_os.h"

#include "DanskTekst.h"

static const char * const NUMBERS[] = 
{
    "tólf",
    "eitt",
    "tvö",
    "þrjú",
    "fjögur",
    "fimm",
    "sex",
    "sjö",
    "átta",
    "níu",
    "tíu",
    "ellefu"
};

void get_hour_string_is(int hour, int minute, char *hourBuffer, size_t length)
{
    memset(hourBuffer, 0, length);
    
    if (minute > 15)
        hour++;
        
    strcat(hourBuffer, NUMBERS[hour % 12]);
}

int get_minute_string_is(int minute, char *line1, char *line2, char *line3, size_t length)
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
	    	strcat(line1, "ein");
	    	strcat(line2, "mínúta");
	    	lines++;
	    	break;
	    case 2:
	    	strcat(line1, "tvær");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 3:
	    	strcat(line1, "þrjár");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 4:
	    	strcat(line1, "fjórar");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 5:
	    	strcat(line1, "fimm");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 6:
	    	strcat(line1, "sex");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 7:
	    	strcat(line1, "sjö");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 8:
	    	strcat(line1, "átta");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 9:
	    	strcat(line1, "níu");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 10:
	    	strcat(line1, "tíu");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 11:
	    	strcat(line1, "ellefu");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 12:
	    	strcat(line1, "tólf");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 13:
	    	strcat(line1, "þrettán");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 14:
	    	strcat(line1, "fjórtán");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 0:
	    case 15:
	    	strcat(line1, "korter");
	    	break;
	    case 16:
	    	strcat(line1, "sextán");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 17:
	    	strcat(line1, "sautján");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 18:
	    	strcat(line1, "átján");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 19:
	    	strcat(line1, "nítján");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 20:
	    	strcat(line1, "tuttugu");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 21:
	    	strcat(line1, "tuttugu og ein");
	    	strcat(line2, "mínóta");
	    	lines++;
	    	break;
	    case 22:
	    	strcat(line1, "tuttugu og tvær");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 23:
	    	strcat(line1, "tuttugu og þrjár");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 24:
	    	strcat(line1, "tuttugu og fjórar");
	    	strcat(line2, "mínútur");
	    	lines++;
	    	break;
	    case 25:
	    	strcat(line1, "tuttugu og fimm");
	    	strcat(line2, "mínútur");
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
    		strcat(line, "yfir");
    		break;
    	case 1:
    		strcat(line, "í hálf");
    		break;
    	case 2:
    		strcat(line, "yfir hálf");
    		break;
    	case 3:
    		strcat(line, "í");
    		break;
    }
    
    return lines;
}

int time_to_4words_is(int hours, int minutes, char *line1, char *line2, char *line3, char *line4, size_t length)
{
	memset(line1, 0, length);
	memset(line2, 0, length);
	memset(line3, 0, length);
	memset(line4, 0, length);

	char hourText[length];
	get_hour_string_is(hours, minutes, hourText, length);
	
	if(minutes == 0)
	{
		strcpy(line1, hourText);
		return 0;
	}
	else if(minutes == 30)
	{
		strcpy(line1, "hálf");
		strcpy(line2, hourText);
		return 1;
	}
	else
	{
		int lineCount = get_minute_string_is(minutes, line1, line2, line3, length);
		char *line = lineCount == 2 ? line3 : line4;
		strcpy(line, hourText);
		return lineCount;
	}
}