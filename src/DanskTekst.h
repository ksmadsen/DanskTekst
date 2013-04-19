#define LANG_DANISH 0
#define LANG_ISLANDIC 1
#define BUFFER_SIZE 44

/********************************* CONFIGURATION ****************************************/

/* If defined; compiles watch as a standalone app, where buttons can be used to change
 * the time. */
//#define DEBUG

/* If defined; compiles a version of the watch where the output is more fuzzy (i.e. less
 * precise */
//#define FUZZY

/* Sets the output language for the watch. See above for supported languages */
#define LANG LANG_DANISH

/********************************* CONFIGURATION ****************************************/


#if LANG == LANG_DANISH

#ifdef FUZZY

#define MY_UUID { 0x7B, 0xDE, 0xD2, 0x8E, 0xF2, 0x02, 0x43, 0x6F, 0xAF, 0x71, 0x06, 0x60, 0x57, 0xA0, 0x64, 0x3F }
#define MY_TITLE "Dansk Fuzzy"

#else

#define MY_UUID { 0xE3, 0x8D, 0xFC, 0x3E, 0x1F, 0x96, 0x4C, 0x0F, 0xAF, 0x6F, 0x78, 0x60, 0xA5, 0x67, 0xEB, 0x27 }
#define MY_TITLE "Dansk Tekst"

#endif /* FUZZY */

#elif LANG == LANG_ISLANDIC

#ifdef FUZZY

#define MY_UUID { 0x43, 0x41, 0x81, 0xC9, 0x33, 0x24, 0x45, 0x1B, 0x81, 0xD9, 0x49, 0x1F, 0x31, 0x0F, 0x4D, 0x8E }
#define MY_TITLE "Íslenskt Fuzzy"

#else

#define MY_UUID { 0xA1, 0x0E, 0x86, 0xC1, 0xAD, 0x71, 0x47, 0xDD, 0x96, 0x36, 0x35, 0x94, 0xF8, 0x07, 0x40, 0xE9 }
#define MY_TITLE "Íslenskur texti"

#endif /* FUZZY */

#endif /* LANG */

#define MAX(a,b) (a > b ? a : b)