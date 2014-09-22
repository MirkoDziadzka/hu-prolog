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


#ifndef __FILES_H_INCLUDED__
#define __FILES_H_INCLUDED__

#if WINDOWS
#include "window.h"
#endif


#define MAXFILES         20
#define BUFLENGTH        85

typedef struct {
     TERM f_name;
     ATOM f_log_name;
     int f_in_ptr;                      /* os file pointer */
     int f_out_ptr;
#if WINDOWS
     int f_win_ptr;                     /* window pointer */
#endif
     TERM f_list_ptr;
     int f_type;
     boolean f_is_eof;   /* end of file */
     boolean f_is_a_tty; /* is a tty */
     boolean f_can_read; /* file is open for read */
     boolean f_can_write; /* file is open for write*/
     boolean f_is_inputfile;  /* file is currently used as inputfile */
     char f_buffer[BUFLENGTH];
     int  f_charpos;
     int  f_length;
     int  f_lineno;
     boolean f_unget;
     } file_type;

extern file_type file_tab[];

#define FNAME(f)          (*(f)).f_name
#define FTYPE(f)          (*(f)).f_type
#define FINPTR(f)         (*(f)).f_in_ptr
#define FOUTPTR(f)        (*(f)).f_out_ptr
#if WINDOWS
#define FWINPTR(f)        (*(f)).f_win_ptr
#endif
#define FLISTPTR(f)       (*(f)).f_list_ptr
#define FLOGNAME(f)       (*(f)).f_log_name

#define EOF(f)            (*(f)).f_is_eof
#define ISTTY(f)          (*(f)).f_is_a_tty
#define CANREAD(f)        (*(f)).f_can_read
#define CANWRITE(f)       (*(f)).f_can_write
#define ISINPUT(f)        (*(f)).f_is_inputfile

#define FBUFFER(f)        (*(f)).f_buffer
#define FCHARPOS(f)       (*(f)).f_charpos
#define FBUFLENGTH(f)     (*(f)).f_length
#define FLINENO(f)        (*(f)).f_lineno
#define FUNGET(f)	  (*(f)).f_unget

#define LINELENGTH       FBUFLENGTH(inputfile)
#define CHARPOS          FCHARPOS(inputfile)
#define LINEBUF          FBUFFER(inputfile)
#define LINENUMBER       FLINENO(inputfile)
#define UNGET		 FUNGET(inputfile)

#define fmode            int
#define write_mode       1
#define read_mode        2
#define read_write       3
#define look_mode        4

typedef file_type *file;
#define STDIN           (&file_tab[0]) 
#define STDOUT          (&file_tab[1])
#define STDERR          (&file_tab[2])
#define STDTRACE        (&file_tab[3])
#if HELP
#define STDHELP        (&file_tab[4])
#endif

#define NOFILE          (&file_tab[-1])
#define ERRFILE         (&file_tab[-2])

#define FIRSTFILE	(&file_tab[0])
#define LASTFILE	(&file_tab[MAXFILES-1])

#define NORMFT           1
#define WINDOWFT         2
#define LISTFT           3

IMPORT file inputfile;
IMPORT file outputfile;
PROTOTYPE(file OpenFile,(TERM,fmode));
PROTOTYPE(void CloseFile,(file));

#endif

/* end of file */
