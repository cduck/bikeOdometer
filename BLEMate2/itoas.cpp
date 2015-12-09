#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "itoas.h"

char* ltoa( long value, char *string, int radix )
{
  char tmp[33];
  char *tp = tmp;
  long i;
  unsigned long v;
  int sign;
  char *sp;
  if ( string == NULL )
  {
    return 0 ;
  }
  if (radix > 36 || radix <= 1)
  {
    return 0 ;
  }
  sign = (radix == 10 && value < 0);
  if (sign)
  {
    v = -value;
  }
  else
  {
    v = (unsigned long)value;
  }
  while (v || tp == tmp)
  {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = i+'0';
    else
      *tp++ = i + 'a' - 10;
  }
  sp = string;
  if (sign)
    *sp++ = '-';
  while (tp > tmp)
    *sp++ = *--tp;
  *sp = 0;
  return string;
}

char* ultoa( unsigned long value, char *string, int radix )
{
  char tmp[33];
  char *tp = tmp;
  long i;
  unsigned long v = value;
  char *sp;
  if ( string == NULL )
  {
    return 0;
  }
  if (radix > 36 || radix <= 1)
  {
    return 0;
  }
 
  while (v || tp == tmp)
  {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = i+'0';
    else
      *tp++ = i + 'a' - 10;
  }
  sp = string;
 
  while (tp > tmp)
    *sp++ = *--tp;
  *sp = 0;
  return string;
}


char* itoa( int value, char *string, int radix )
{
  return ltoa( value, string, radix ) ;
}

char* utoa( unsigned long value, char *string, int radix )
{
  return ultoa( value, string, radix ) ;
}

/*
The dtostrf() function converts the double value passed in val into an ASCII
representationthat will be stored under s. The caller is responsible for
providing sufficient storage in s.

Conversion is done in the format "[-]d.ddd". The minimum field width of the
output string (including the '.' and the possible sign for negative values) is
given in width, and prec determines the number of digits after the decimal
sign. width is signed value, negative for left adjustment.

The dtostrf() function returns the pointer to the converted string s.
*/
char * dtostrf(double number, signed char width, unsigned char prec, char *s)
 {

    if(isnan(number)) {
        strcpy(s, "nan");
        return s;
    }
    if(isinf(number)) {
        strcpy(s, "inf");
        return s;
    }

    if(number > 4294967040.0 || number < -4294967040.0) {
        strcpy(s, "ovf");
        return s;
    }
    char* out = s;
    // Handle negative numbers
    if(number < 0.0) {
        *out = '-';
        ++out;
        number = -number;
    }

    // Round correctly so that print(1.999, 2) prints as "2.00"
    double rounding = 0.5;
    for(uint8_t i = 0; i < prec; ++i)
        rounding /= 10.0;

    number += rounding;

    // Extract the integer part of the number and print it
    unsigned long int_part = (unsigned long) number;
    double remainder = number - (double) int_part;
    out += sprintf(out, "%d", int_part);

    // Print the decimal point, but only if there are digits beyond
    if(prec > 0) {
        *out = '.';
        ++out;
    }

    while(prec-- > 0) {
        remainder *= 10.0;
        if((int)remainder == 0){
                *out = '0';
                 ++out;
        }
    }
    sprintf(out, "%d", (int) remainder);

    return s;
}
