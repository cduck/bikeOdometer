#ifndef __ITOAS_H__
#define __ITOAS_H__

#include "cstdlib"

char* utoa( unsigned long value, char *string, int radix );
char* itoa( int value, char *string, int radix );
char* ultoa( unsigned long value, char *string, int radix );
char* ltoa( long value, char *string, int radix );
char* dtostrf(double __val, signed char __width, unsigned char __prec, char * __s);

#endif /* #ifndef __ITOAS_H__ */