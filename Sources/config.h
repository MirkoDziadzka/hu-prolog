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

#ifndef __CONFIG_H_INCLUDED__
#define __CONFIG_H_INCLUDED__ 	1

/* 
** The generation of HU-Prolog is parametrized by
**
** wordsize:                    BIT16 BIT32
** operating system:            UNIX MS_DOS VMS
** addressmode                  POINTERMODE BYTEMODE WORDMODE 
** arithmetic:                  LONGARITH REALARITH FUNARITH
** unification mode:            OCCUR_CHECK (switchable or not)
** programming style:           QUICK_BUT_DIRTY
**
** restricted memory:           SMALLVERSION
** 
** extensions:                  WINDOWS   Window-Interface 
**                              USER      C-Interface
**                              HELP      Online Help
**				SAVE	  save/1
**				USE_HUPROLOG_ENV_VAR 
**				EXTENDED_ASCII  LATIN1
**				STRINGS_AS_INT_LISTS	 
**
**				HIGHER_ORDER (*)
**				NO_TRACE (*)
**				NEW_WINDOWS  (*)
**				USER_UNIFY   (*)
**				(*) Don't use it !
*/


#ifndef PRIVATE_VERSION
#define HIGHER_ORDER		0
#define USER_UNIFY		0
#define NEW_WINDOWS		0
#define NO_TRACE		0
#endif


#if defined(SYSV) || defined(BSD) || defined(POSIX)
# define UNIX	1
#endif
#ifdef __TURBOC__
# define TURBOC 1
# define SIGNAL_RETURNS_VOID	1
#endif
#if defined(MSC) || defined(TURBOC)
# define MS_DOS 1
#endif

#if UNIX
#       define BIT32 		1
#       define POINTERMODE 	1
#       define MAXDEPTH 	5000
#ifndef MEMORYSIZE
#	define MEMORYSIZE	(1024 * 1024)
#endif
#endif
#if MS_DOS
#       define BIT16 		1
#       define BYTEMODE 	1
#       define SMALLVERSION 	1
#       define MAXDEPTH 	300
#	define MEMORYSIZE	300
#	define EXTENDED_ASCII	1
#endif


#define LONGARITH 1
#define REALARITH 1
#define INLINE 1
#define USER 1
#define DEBUG 1

#ifndef HELP
#define HELP	1
#endif



/*******************************************************/

#ifndef REALARITH
#       define REALARITH 0
#endif

#ifndef LONGARITH
#       define LONGARITH 0
#endif

#ifndef FUNARITH
#       define FUNARITH 0
#endif

#ifdef lint
#       define BIT32 1
#       define UNIX 1
#       define POINTEROFFSET 1
#	define QUICK_BUT_DIRTY 0
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

/* Operating Systems */

#ifndef VMS
#define VMS 0
#endif

#ifndef UNIX
#define UNIX 0
#endif

#ifndef MS_DOS
#define MS_DOS 0
#endif

#ifndef MSC
#define MSC 0
#endif

#ifndef TURBOC
#define TURBOC 0
#endif

/* System Options */

#ifndef WINDOWS
#define WINDOWS 1
#endif

/* Word Size */

#ifndef BIT16
#define BIT16 0
#endif

#ifndef BIT32
#define BIT32 0
#endif

/* Unification Mode */

#ifndef OCCUR_CHECK
#define OCCUR_CHECK 0
#endif

#ifndef BYTEMODE
#define BYTEMODE 0
#endif

#ifndef WORDMODE
#define WORDMODE 0
#endif

#ifndef POINTERMODE
#define POINTERMODE 0
#endif

#ifndef INLINE
#define INLINE 1
#endif

#ifndef MAXDEPTH 
#define MAXDEPTH 200
#endif

#ifndef SMALLVERSION
#define SMALLVERSION 0
#endif

#ifndef QUICK_BUT_DIRTY
#define QUICK_BUT_DIRTY 1
#endif

#ifndef SAVE
#define SAVE 1
#endif

#ifndef LATIN1
#define LATIN1 0
#endif

#ifndef EXTENDED_ASCII
#define EXTENDED_ASCII 0
#endif

#ifndef STRINGS_AS_INT_LISTS
#define STRINGS_AS_INT_LISTS	1
#endif

#ifndef BSD
#define BSD	0
#endif

#ifndef SYSV
#define SYSV	0
#endif

#ifndef POSIX
#define POSIX	0
#endif

#ifndef NO_VOID_CAST
#define NO_VOID_CAST 0
#endif

#ifndef NO_PROTOTYPE
#define NO_PROTOTYPE 0
#endif

#ifndef NO_TRACE
#define NO_TRACE	0
#endif

#ifndef SIGNAL_RETURNS_VOID
#if __STDC__
#  define SIGNAL_RETURNS_VOID	1
# else
#  define SIGNAL_RETURNS_VOID	0
# endif
#endif

#ifndef NEW_WINDOWS
# if WINDOWS
#  define NEW_WINDOWS		1
# else
#  define NEW_WINDOWS		0
# endif
#endif

#ifndef USE_HUPROLOG_ENV_VAR
#define USE_HUPROLOG_ENV_VAR	1
#endif

#ifndef USER_UNIFY
#define USER_UNIFY		0
#endif

#if (POINTERMODE + BYTEMODE + WORDMODE) != 1
#include "one of the above should be defined!"
#include "are you sure, that you have choose the right parameters"
#include "in Makefile ????"
#endif

#if TURBOC
# ifndef __HUGE__
#  include "sorry, modell HUGE expected"
#  include "try -mh or change in O/C/C"
# endif
#endif

#endif

/* end of file */
