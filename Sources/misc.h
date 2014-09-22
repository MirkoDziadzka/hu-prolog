/**********************************************************************
***********************************************************************
**                                                                   **
**  HU-Prolog         Public Domain Version       Release  2.029     **
**                                                                   **
**  Author(s): 87-89  Christian Horn, Mirko Dziadzka, Matthias Horn  **
**             90-95  Mirko Dziadzka                                 **
**                    Fachhoschule Furtwangen, Allgemeine Informatik **
**                    email: dziadzka@ai-lab.fh-furtwangen.de        **
**                                                                   **
**  HU-Prolog 2.029 can be copied, modified and distributed freely   **
**  for any non-commercial purposes under the conditions that all    **
**  files include this header and that the message                   **
**  "HU-Prolog public domain version" and the names of the authors   **
**  will be displayed, when HU-Prolog is called.                     **
**                                                                   **
***********************************************************************
**********************************************************************/


#ifndef __MISC_H_INCLUDED__
#define __MISC_H_INCLUDED__	1

#ifdef NO_PROTOTYPE
#	define PROTOTYPE(NAME,ARG_LIST)	IMPORT NAME ()
#endif
#ifndef NO_PROTOTYPE
#	define PROTOTYPE(NAME,ARG_LIST)	NAME ARG_LIST
#endif

#define IMPORT  extern
#define FORWARD extern
#define LOCAL   static
#define GLOBAL  /* global */
#define REGISTER

#ifdef __GNUC__
# define VOLATILE	volatile
#else
# define VOLATILE	/* volatile */
#endif


#define maxlong (0x7fffffffL)
#define minlong (-maxlong)

typedef unsigned int card;

#if BIT16 
#define maxint (0x7fffL)            /* maximal integer */
#define minint (-maxint)           /* minimal integer */
typedef unsigned int bit16;
#endif

#if BIT32
#define maxint maxlong
#define minint minlong
typedef unsigned short bit16;
#endif

#if REALARITH
typedef double REAL;
#define REALSIZE        (sizeof(REAL) / sizeof(int))
#endif
#if LONGARITH
typedef long LONG;
#define LONGSIZE        (sizeof(LONG) / sizeof(int))
#endif


#if BIT32
#define MAX_NAME_LEN    (128 * 1024)
#endif

#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN	512
#endif
 
#define MAXARITY 127     /* max. arity for atoms */
#define MAXPREC 2047     /* Max. operator precedence. */
#define SUBPREC  999     /* Max. prec. for subterms. */

#define MAXVARS    200    /* max. num. of variables for r/w */
#define VARLIMIT  1000    /* max. strlen of all vars a term */

#define WRITEDEPTH 250    /* Max. nesting depth for write */
#define WRITELENGTH 250   /* Max. list length for write */

#if BIT32
#define READSIZE  (16 * 1024)    /* Size of read stack */
#define READDEPTH (8 * 1024)    /* Max.nesting depth */
#endif
#if BIT16
#define READSIZE  250 
#define READDEPTH 250
#endif

#if BYTEMODE && !BIT16
sorry, BYTEMODE expect BIT16
#endif

#endif

/* end of file */
