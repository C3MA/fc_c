/*
 *  hwal.c
 *
 * Hardware abstraction layer
 * ==========================
 *
 * Implementation for Chibios
 * 
 *  Created by ollo on 30.08.13.
 *  Copyright 2013 C3MA. All rights reserved.
 *
 */

#include <string.h>
#include <stdarg.h>

#include "ch.h"

#include "hwal.h"
#include "customHwal.h"

#include "ff.h"

#define MAXFILEDESCRIPTORS	10

#define MAX_FILLER 11

/* attribute, needed to store the stream to print on */
static BaseSequentialStream *gChp = NULL;


/* a small mapping table to abstract from the filedescriptors */
FIL*	fd_mappingtable[MAXFILEDESCRIPTORS];
int	fdNextFreecount=1; /* do not use the number zero, as zero marks errors while opening */

extern int hwal_fopen(char *filename, char* type)
{
	int usedMapIndex = fdNextFreecount;
	if (usedMapIndex > MAXFILEDESCRIPTORS)
	{
		return 0; /* reached the maximum possible files */
	}

	/* secure, that each map entry is only used once: */
	fdNextFreecount++;
	
	/* human counting adapted to the array (therefore the -1) */
	/*FIXME only allow reading for an easy handling */
	if ( f_open( fd_mappingtable[usedMapIndex - 1], (TCHAR*) filename, FA_READ) != FR_OK)
	{
		fd_mappingtable[usedMapIndex -1] = NULL;
		return 0; /* damn it did not work */
	}
	
	return usedMapIndex;
}
	
extern int hwal_fread(void* buffer, int length, int filedescriptor)
{
	int br;
	if (f_read( fd_mappingtable[filedescriptor -1], (TCHAR*) buffer, length,(UINT*) &br) != FR_OK)
	{
		return 0; /* problems, return zero as problematic length */
	}
	else
	{
		return br; /* return the number of read bytes */
	}
}
	
	
extern void hwal_fclose(int filedescriptor)
{
	f_close( fd_mappingtable[filedescriptor -1] );
	/* FIXME free the entries in the mapping table */
}

extern void hwal_memset(void *buffer, int item, int length)
{
	int i;
	uint8_t* b = (uint8_t*) buffer;
	for(i=0; i < length; i++)
	{
		b[i] = item;
	}
}

extern void hwal_memcpy(const void *s1, const void* s2, int length)
{
	int i;
	uint8_t* buffs1 = (uint8_t*) s1;
	uint8_t* buffs2 = (uint8_t*) s2;
	for(i=0; i < length; i++)
	{
		buffs1[i] = buffs2[i];
	}	
}

extern int hwal_strlen(char* text)
{
	int i =0;

	while (text[i] != '\0') {
		i++;
	}
	return i;
}

/* ----------- exported from Chibios: chprintf.c ------------ */
static char *long_to_string_with_divisor(char *p,
                                         long num,
                                         unsigned radix,
                                         long divisor) {
  int i;
  char *q;
  long l, ll;

  l = num;
  if (divisor == 0) {
    ll = num;
  } else {
    ll = divisor;
  }

  q = p + MAX_FILLER;
  do {
    i = (int)(l % radix);
    i += '0';
    if (i > '9')
      i += 'A' - '0' - 10;
    *--q = i;
    l /= radix;
  } while ((ll /= radix) != 0);

  i = (int)(p + MAX_FILLER - q);
  do
    *p++ = *q++;
  while (--i);

  return p;
}

static char *ltoa(char *p, long num, unsigned radix) {

  return long_to_string_with_divisor(p, num, radix, 0);
}

#if CHPRINTF_USE_FLOAT
static char *ftoa(char *p, double num) {
  long l;
  unsigned long precision = FLOAT_PRECISION;

  l = num;
  p = long_to_string_with_divisor(p, l, 10, 0);
  *p++ = '.';
  l = (num - l) * precision;
  return long_to_string_with_divisor(p, l, 10, precision / 10);
}
#endif

void chvprintf(BaseSequentialStream *chp, const char *fmt, va_list ap) {
  char *p, *s, c, filler;
  int i, precision, width;
  bool_t is_long, left_align;
  long l;
#if CHPRINTF_USE_FLOAT
  float f;
  char tmpbuf[2*MAX_FILLER + 1];
#else
  char tmpbuf[MAX_FILLER + 1];
#endif

  while (TRUE) {
    c = *fmt++;
    if (c == 0) {
      return;
    }
    if (c != '%') {
      chSequentialStreamPut(chp, (uint8_t)c);
      continue;
    }
    p = tmpbuf;
    s = tmpbuf;
    left_align = FALSE;
    if (*fmt == '-') {
      fmt++;
      left_align = TRUE;
    }
    filler = ' ';
    if ((*fmt == '.') || (*fmt == '0')) {
      fmt++;
      filler = '0';
    }
    width = 0;
    while (TRUE) {
      c = *fmt++;
      if (c >= '0' && c <= '9')
        c -= '0';
      else if (c == '*')
        c = va_arg(ap, int);
      else
        break;
      width = width * 10 + c;
    }
    precision = 0;
    if (c == '.') {
      while (TRUE) {
        c = *fmt++;
        if (c >= '0' && c <= '9')
          c -= '0';
        else if (c == '*')
          c = va_arg(ap, int);
        else
          break;
        precision *= 10;
        precision += c;
      }
    }
    /* Long modifier.*/
    if (c == 'l' || c == 'L') {
      is_long = TRUE;
      if (*fmt)
        c = *fmt++;
    }
    else
      is_long = (c >= 'A') && (c <= 'Z');

    /* Command decoding.*/
    switch (c) {
    case 'c':
      filler = ' ';
      *p++ = va_arg(ap, int);
      break;
    case 's':
      filler = ' ';
      if ((s = va_arg(ap, char *)) == 0)
        s = "(null)";
      if (precision == 0)
        precision = 32767;
      for (p = s; *p && (--precision >= 0); p++)
        ;
      break;
    case 'D':
    case 'd':
    case 'I':
    case 'i':
      if (is_long)
        l = va_arg(ap, long);
      else
        l = va_arg(ap, int);
      if (l < 0) {
        *p++ = '-';
        l = -l;
      }
      p = ltoa(p, l, 10);
      break;
#if CHPRINTF_USE_FLOAT
    case 'f':
      f = (float) va_arg(ap, double);
      if (f < 0) {
        *p++ = '-';
        f = -f;
      }
      p = ftoa(p, f);
      break;
#endif
    case 'X':
    case 'x':
      c = 16;
      goto unsigned_common;
    case 'U':
    case 'u':
      c = 10;
      goto unsigned_common;
    case 'O':
    case 'o':
      c = 8;
unsigned_common:
      if (is_long)
        l = va_arg(ap, unsigned long);
      else
        l = va_arg(ap, unsigned int);
      p = ltoa(p, l, c);
      break;
    default:
      *p++ = c;
      break;
    }
    i = (int)(p - s);
    if ((width -= i) < 0)
      width = 0;
    if (left_align == FALSE)
      width = -width;
    if (width < 0) {
      if (*s == '-' && filler == '0') {
        chSequentialStreamPut(chp, (uint8_t)*s++);
        i--;
      }
      do
        chSequentialStreamPut(chp, (uint8_t)filler);
      while (++width != 0);
    }
    while (--i >= 0)
      chSequentialStreamPut(chp, (uint8_t)*s++);

    while (width) {
      chSequentialStreamPut(chp, (uint8_t)filler);
      width--;
    }
  }
}

extern void hwal_debug(char* codefile, int linenumber, char* text, ...)
{
	 va_list ap;
	/* when no stream is definded, to print on, then quit the function */
	if (gChp == NULL)
		return;
	chprintf(gChp, "%s:%d ", codefile, linenumber);
	va_start(ap, text);
	chvprintf(gChp, text, ap);
	va_end(ap);

	chprintf(gChp, "\r\n");
}

extern void hwal_init(BaseSequentialStream *chp)
{
	gChp = chp;
	chprintf(gChp, "Hardware Abstraction Layer INITIALIZED!\r\n");
}
