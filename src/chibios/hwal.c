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

/* Socket requirements */
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "hwal_net.h" /* needed for noneblocking TCP accept */

#define MAXFILEDESCRIPTORS	10

#define MAX_FILLER 11

#define	TCPINPUT_MAILBOX_SIZE	4

/** @var gTCPincommingBuf
 *	@var gTCPinMailbox
 *	@brief Internal mailbox, to indicate something new to read has arrived
 */
static uint32_t gTCPincommingBuf[TCPINPUT_MAILBOX_SIZE];
static MAILBOX_DECL(gTCPinMailbox, gTCPincommingBuf, TCPINPUT_MAILBOX_SIZE);

/** @var gTCPinProbBuf
 *	@var gTCPinProblemMailbox
 *	@brief Internal mailbox, to indicate problems on errors
 */
static uint32_t gTCPinProbBuf[TCPINPUT_MAILBOX_SIZE];
static MAILBOX_DECL(gTCPinProblemMailbox, gTCPinProbBuf, TCPINPUT_MAILBOX_SIZE);

/* attribute, needed to store the stream to print on */
static BaseSequentialStream *gChp = NULL;


/* a small mapping table to abstract from the filedescriptors */
FIL	fd_mappingtable[MAXFILEDESCRIPTORS];
int	fdNextFreecount=1; /* do not use the number zero, as zero marks errors while opening */

extern int hwal_fopen(char *filename, char* type)
{
	int usedMapIndex = fdNextFreecount;

	(void) type; /* make GCC happy and show it, that we don't need this argument here */

	if (usedMapIndex > MAXFILEDESCRIPTORS)
	{
		return 0; /* reached the maximum possible files */
	}

	/* secure, that each map entry is only used once: */
	fdNextFreecount++;
	
	/* human counting adapted to the array (therefore the -1) */
	/*FIXME only allow reading for an easy handling */
	if ( f_open( &(fd_mappingtable[usedMapIndex - 1]), (TCHAR*) filename, FA_READ) != FR_OK)
	{
		fdNextFreecount--;
		return 0; /* damn it did not work */
	}
	/* DEBUG_PLINE("Open file with index %d and descriptor %d", usedMapIndex, 
			fd_mappingtable[usedMapIndex - 1]); */
	return usedMapIndex;
}
	
extern int hwal_fread(void* buffer, int length, int filedescriptor)
{
	int br;
	FRESULT status;
	status = f_read( &(fd_mappingtable[filedescriptor -1]), (TCHAR*) buffer, length,(UINT*) &br);
	/* DEBUG_PLINE("Read returned %d ", status ); */
	if (status != FR_OK)
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
	f_close( &(fd_mappingtable[filedescriptor -1]) );
	/* FIXME free the entries in the mapping table */
	fdNextFreecount--;
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

extern void hwal_memcpy(void *s1, void* s2, int length)
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

#ifndef _CHPRINTF_H_
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
#endif

extern void hwal_debug(char* codefile, int linenumber, char* text, ...)
{
	 va_list ap;
	/* when no stream is defined, to print on, then quit the function */
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
#ifdef PRINT_DEBUG
	chprintf(gChp, "Hardware Abstraction Layer INITIALIZED!\r\n");
#else
	chprintf(gChp, "DEBUGGING is not possible!\r\n");
#endif
}

extern void* hwal_malloc(int size)
{
	return chHeapAlloc(NULL, size);
}

extern void hwal_free(void* memory)
{
	if (memory != NULL)
		chHeapFree(memory);
}

/** @fn static void socket_callback(struct netconn *conn, enum netconn_evt evnt, u16_t len)
 * Callback, that is triggered on changes of the TCP socket.
 *
 * Source:
 * http://www.ecoscentric.com/ecospro/doc/html/ref/lwip-api-sequential-netconn-new-with-callback.html
 *
 * @param[in]   conn    Network connection
 * @param[in]   evnt    Event, that occurred
 * @param[in]   len     Length of the information, that is sent or received
 */
static void socket_callback(struct netconn *conn, enum netconn_evt evnt, u16_t len)
{
	switch (evnt) {
		case NETCONN_EVT_RCVPLUS:
		        /* Occurs at:
		         * Used when new incoming data from a remote peer arrives.
		         * The amount of data received is passed in len.
		         * If len is 0 then a connection event has occurred:
		         *  this may be an error, the acceptance of a connection
		         *  for a listening connection (called for the listening connection), or deletion of the connection.
		         *
		         **/
			chSysLock();
			/* Put new events always in the first place */
			chMBPostAheadI(&gTCPinMailbox, (uint32_t) conn);
			chSysUnlock();
			break;
		case NETCONN_EVT_RCVMINUS:
		        /* Used when new incoming data from a remote peer has been received and accepted by higher layers.
		         * The amount of data accepted is passed in len. */
			DEBUG_PLINE("Read some bytes at %d [conn %d], exactly %d", conn, evnt, len);
			chSysLock();
                        /* Put new events always in the first place */
                        chMBPostAheadI(&gTCPinMailbox, (uint32_t) conn);
                        chSysUnlock();
			break;
		case NETCONN_EVT_SENDPLUS:
			/* DEBUG_PLINE("write some bytes at %d [conn %d], exactly %d", conn, evnt, len); */
			break;
		case NETCONN_EVT_SENDMINUS:
			/* DEBUG_PLINE("Write too mutch bytes at %d [conn %d], exactly %d", conn, evnt, len); */
			break;
		case NETCONN_EVT_ERROR:
			DEBUG_PLINE("Error with bytes at %d [conn %d], exactly %d", conn, evnt, len);
			break;
		default:
			DEBUG_PLINE("Event %d [conn %d], with %d bytes", evnt, conn, len);
                        chSysLock();
                        /* Put new events always in the first place */
                        chMBPostAheadI(&gTCPinProblemMailbox, (uint32_t) conn);
                        chSysUnlock();
			break;
	}
}

extern int hwal_socket_tcp_new(int port, int maximumClients)
{
	(void) maximumClients;
	/* Prepare the Mailbox for the callbackfunction to indicate new packets */
	chMBInit(&gTCPinMailbox, (msg_t *)gTCPincommingBuf, TCPINPUT_MAILBOX_SIZE);
	chMBInit(&gTCPinProblemMailbox, (msg_t *)gTCPinProbBuf, TCPINPUT_MAILBOX_SIZE);
	
	DEBUG_PLINE("TCP Read Mailbox is ready to use! And prepared for %d messages", TCPINPUT_MAILBOX_SIZE);
	
	/* Create a new socket, that handles its accepts in a seperate thread */
	return hwalnet_new_socket(port, &socket_callback);
}

extern void hwal_socket_tcp_close(int socketfd)
{
	struct netconn *conn = (struct netconn *) socketfd;
	netconn_delete(conn);
}

extern int hwal_socket_tcp_accet(int socketfd)
{	
	return hwalnet_new_client(socketfd);
}

extern int hwal_socket_tcp_read(int clientSocket, uint8_t* workingMem, uint32_t workingMemorySize)
{
	err_t			err;
	int				i;
	struct netbuf	*inbuf;
	char			*buf;
	u16_t			buflen				= 0;
	int				newMessages			= 0;
	int				newMsgProbMailbox	= 0;
	msg_t			msg1;
	msg_t			status;
	struct netconn	*conn = (struct netconn *) clientSocket;
	
	/* Check incoming parameters */
	if (clientSocket == 0 || workingMem == 0 || workingMemorySize == 0)
	{
		return -2;
	}
	
	DEBUG_PLINE("Checking Mailbox supervised - callback");
	/* Use nonblocking function to count incoming messages (if there are new bytes to read) */
	newMsgProbMailbox = chMBGetUsedCountI( &gTCPinProblemMailbox );
	
	newMessages = chMBGetUsedCountI( &gTCPinMailbox );
	
	if (newMessages <= 0)
	{
		/* ------ check if the TCP session is still active -------- */
		err = ERR_TIMEOUT;
		for (i=0; i < newMsgProbMailbox; i++)
		{
			
			status = chMBFetch(&gTCPinProblemMailbox, &msg1, TIME_INFINITE);
			if (status == RDY_OK)
			{
				chSysLock();
				if (((uint32_t) msg1) ==  (uint32_t) clientSocket)
				{
					err = ERR_OK;
				}
				chSysUnlock();
			}
		}
		
		/* no suitable thread found */
		if (err == ERR_OK)
		{
			DEBUG_PLINE("Socket %X seems disconnected", clientSocket);
			return 0; /* connection closed by the client */
		}
		
		/* There are no new messages found */
		return -1;
	}
	
	DEBUG_PLINE("Got %d messages", newMessages);
	/* ------ Search for new bytes to read -------- */
	err = ERR_TIMEOUT;
	for (i=0; i < newMessages; i++)
	{
		
		status = chMBFetch(&gTCPinMailbox, &msg1, TIME_INFINITE);
		if (status == RDY_OK)
		{
			chSysLock();
			if (((uint32_t) msg1) ==  (uint32_t) clientSocket)
			{
				err = ERR_OK;
			}
			else
			{
				DEBUG_PLINE("[%d. / %d ] Socket %X expected, but %X has new bytes", (i + 1), (newMessages + 1),
							clientSocket, ((uint32_t) msg1));
				chMBPostI(&gTCPinMailbox, (uint32_t) msg1);				
			}
			chSysUnlock();
		}
	}
	
	/* no suitable thread found */
	if (err != ERR_OK)
	{
		return -1;
	}
	
	DEBUG_PLINE("%d reading...", conn);
	err = netconn_recv(conn, &inbuf);
	DEBUG_PLINE("%d read returned %d", conn, err);
	
	switch (err)
	{
	case ERR_OK:
	{
		netbuf_data(inbuf, (void **)&buf, &buflen);
		DEBUG_PLINE("Buffer found %d bytes and returned %d", buflen, err);
		if (err != ERR_OK)
		{
			buflen = 0;
		}
		else if (buflen == 0)
		{
			buflen = -1; /* nothing new */
		}
		else
		{
			if (buflen <= workingMemorySize)
			{
				/* copy content to the outputbuffer */
				hwal_memcpy(workingMem, buf, buflen);			
			}
			else
			{
				/* resque as mutch memory as possible */
				hwal_memcpy(workingMem, buf, workingMemorySize);
				buflen = -2; /* There was not enough memory */
			}
		}
		netbuf_delete(inbuf); /* free the memory, provided by the netcon_recv function */
		return buflen;
	}
	case ERR_TIMEOUT:
		return -1; /* nothing new */
	default:
		return 0; /* connection closed by the client */
	}

}

extern int hwal_socket_tcp_write(int clientSocket, uint8_t* data, uint32_t size)
{
	err_t err;
	struct netconn *conn = (struct netconn *) clientSocket;
	
	err = netconn_write(conn, data, size, NETCONN_NOCOPY);
	
	DEBUG_PLINE("%d write returned %d", conn, err);
	
	if (err == ERR_OK)
	{
		return size;
	}
	else
	{
		return -1;
	}
}
