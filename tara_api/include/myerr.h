#ifndef _MYERR_H_
#define _MYERR_H_

#include <errno.h>

typedef enum {
	ERROR_NONE=0,                  // no error
	ERROR_UNKNOWN=-1024,           // unknown error
	ERROR_AGAIN=-1025,             //
	ERROR_FULL=-1026,              //
	ERROR_EMPTY=-1027,             // no data available
	ERROR_FAIL=-1028,              // operation failed
	ERROR_INVALARG=-1029,          // invalid argument
	ERROR_READFAIL=-1030,          // get data failed
	ERROR_UNSUPPORTED=-1031,       // not supported operation
	ERROR_TIMEOUT=-1032,           // operation timeout reached
	ERROR_DUPLICATE=-1033,         // data not changed
	ERROR_OUTOFMEMORY=-1034,       // out of memory
	ERROR_NOVIDEO=-1035            // no input signal
} TARA_ERROR_CODE;

#endif
