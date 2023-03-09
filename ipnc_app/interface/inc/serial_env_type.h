#ifndef _SERIAL_ENV_TYPE_H_
#define _SERIAL_ENV_TYPE_H_

typedef struct _serialAttr{
	int BaudRate;
	int DataBits;
	int Parity;
	int StopBits;
	int TxDataType;
} serialAttr_s;

#endif /*_SERIAL_ENV_TYPE_H_*/
