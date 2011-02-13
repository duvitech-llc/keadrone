/*
 * retargetPrintf.c
 *
 *  Created on: 28 jan 2011
 *      Author: admin
 */

//#include "uart0.h"

// Include stdio.h to pull in __REDLIB_INTERFACE_VERSION__
#include <stdio.h>
#include "type.h"
#define SEMIHOST_ENABLED
#ifdef SEMIHOST_ENABLED
#if (__REDLIB_INTERFACE_VERSION__ >= 20000)
// We are using new Redlib_v2 semihosting interface
	#define WRITEFUNC __sys_write
	#define READFUNC __sys_readc
#else
// We are using original Redlib semihosting interface
	#define WRITEFUNC __write
	#define READFUNC __readc
#endif


// Function __write() / __sys_write
//
// Called by bottom level of printf routine within RedLib C library to write
// a character. With the default semihosting stub, this would write the character
// to the debugger console window . But this version writes
// the character to the LPC1768/RDB1768 UART.
int WRITEFUNC (int iFileHandle, char *pcBuffer, int iLength)
{
	unsigned int i;
	//for (i = 0; i<iLength; i++)
	//{
		UARTSend(3, (uint8_t *) pcBuffer, iLength);
//		UART0_Sendchar(pcBuffer[i]); // print each character
	//}
	return iLength;
}

// Function __readc() / __sys_readc
//
// Called by bottom level of scanf routine within RedLib C library to read
// a character. With the default semihosting stub, this would read the character
// from the debugger console window (which acts as stdin). But this version reads
// the character from the LPC1768/RDB1768 UART.
int READFUNC (void)
{
//	char c = UART0_Getchar();
//	return (int)c;
}
#endif
