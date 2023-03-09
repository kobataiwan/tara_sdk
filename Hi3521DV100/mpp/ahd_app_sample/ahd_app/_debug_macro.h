#ifndef __DEBUG_MACRO_H__
#define __DEBUG_MACRO_H__

#define INFO_FLICKER 	5
#define INFO_REVERSE 	7
#define INFO_STRIKE  	9
#define INFO_RED	    31
#define INFO_GREEN    	32
#define INFO_YELLOW   	33
#define INFO_BLUE     	34
#define INFO_PURPLE   	35
#define INFO_TEAL   	36
#define INFO_GRAY   	37
#define INFO_RED_BG   	41
#define INFO_GREEN_BG   42
#define INFO_YELOOW_BG  43
#define INFO_BLUE_BG   	44
#define INFO_PURPLE_BG  45
#define INFO_TEAL_BG   	46
#define INFO_GRAY_BG   	47

#define STR_HELPER_(x) #x
#define STR_(x) STR_HELPER_(x)

#include <string.h>
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DBG_INFO(...) _os_dbg_info(__FILENAME__, STR_(__LINE__), __FUNCTION__, __VA_ARGS__)
#define DBG_ERR(...) _os_dbg_error(__FILENAME__, STR_(__LINE__), __FUNCTION__, __VA_ARGS__ )
#define COLOR_DBG_INFO(...) _os_dbg_color_info(__FILENAME__, STR_(__LINE__), __FUNCTION__, __VA_ARGS__)
///////// for debug

#include <stdarg.h>
static void _os_dbg_error( const char *dbg1, const char *dbg2, const char *dbg3, const char *fmt, ... )
{
	char buffer[255];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	va_end(ap);

	char *pS = buffer;
	while(*pS) { if( *pS == '\n' ) *pS= ' '; pS++; }

	fprintf(stdout,"[APP] Error :[%s:%s] %s : %s\n", dbg1, dbg2, dbg3, buffer);
}

static void _os_dbg_info( const char *dbg1, const char *dbg2, const char *dbg3, const char *fmt, ... )
{
	char buffer[255];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	va_end(ap);

	char *pS = buffer;
	while(*pS) { if( *pS == '\n' ) *pS= ' '; pS++; }

	fprintf(stdout,"[APP] Info :[%s:%s] %s : %s\n", dbg1, dbg2, dbg3, buffer);
}

static void _os_dbg_color_info( const char *dbg1, const char *dbg2, const char *dbg3, const char color, const char *fmt, ... )
{
	char buffer[255];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	va_end(ap);

	char *pS = buffer;
	while(*pS) { if( *pS == '\n' ) *pS= ' '; pS++; }

	fprintf(stdout,"[APP] Info :[%s:%s] %s : %s\n", dbg1, dbg2, dbg3, buffer);
	//printf("\033[32m\033[1m[APP]\033[0m Info  : [\033[32m\033[1m%s\033[0m:\033[33m\033[1m%s\033[0m] : [\033[37m%s\033[0m] : \033[%dm\033[1m%s\033[0m \n",  dbg1, dbg2, dbg3, color, buffer );
}

/////////

#endif
