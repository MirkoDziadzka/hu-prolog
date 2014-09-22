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


#ifndef __EXTERNS_H_INCLUDED__
#define __EXTERNS_H_INCLUDED__	1

/* Schnittstelle zum Betriebssystem */

#include <setjmp.h>

PROTOTYPE(int open,(char *,int));
PROTOTYPE(int creat,(char *,int));
PROTOTYPE(int close,(int));
PROTOTYPE(int read,(int,char *,int));
PROTOTYPE(int write,(int,char *,int));
PROTOTYPE(int isatty,(int));
PROTOTYPE(int unlink,(char *));
PROTOTYPE(long lseek,(int,long,int));
PROTOTYPE(int access,(char *, int));

#if UNIX 
PROTOTYPE(int kill,(int,int));
PROTOTYPE(int getpid,(void));
PROTOTYPE(int pause,(void));
#ifndef NO_TERMCAP
/* TERMCAP */
PROTOTYPE(int  tgetent,(char *));
PROTOTYPE(char *tgetstr,(char *, char **));
PROTOTYPE(int  tgetnum,(char *));
PROTOTYPE(char *tgoto,(char *, int, int));
#endif
#endif

#if SAVE
PROTOTYPE(int execlp,(char *, char *, ...));
#endif

PROTOTYPE(void exit,(int));
PROTOTYPE(char *getenv,(char *));

PROTOTYPE(char *strcpy,(char *,char*));
PROTOTYPE(char *strncpy,(char *,char*, int));
PROTOTYPE(int  strcmp,(char *,char*));
/*
PROTOTYPE(int qsort, (void *,int,int, (int)* ));
*/

#ifndef __GNUC__
PROTOTYPE(char *memcpy,(char *, char *, int));
PROTOTYPE(int  strlen,(char *));
#endif


/*
** arith.c
*/
PROTOTYPE(boolean DOREDUCE,(TERM, TERM, boolean));
PROTOTYPE(TERM DOEVAL,(TERM, ENV));
PROTOTYPE(boolean DOACOMP,(void));
PROTOTYPE(boolean DOIS,(void));
PROTOTYPE(boolean DODASS,(void));
PROTOTYPE(int INTVALUE,(TERM));
PROTOTYPE(void Init_Arith,(void));

/*
** atoms.c
*/
PROTOTYPE(ATOM LOOKUP,(string,int,boolean,boolean));
PROTOTYPE(ATOM LOOKATOM,(ATOM,int,boolean));
PROTOTYPE(ATOM atom,(TERM));
PROTOTYPE(ATOM nameof,(TERM));
PROTOTYPE(ATOM copyatom,(ATOM));
PROTOTYPE(ATOM GetAtom,(ATOM));
PROTOTYPE(TERM LISTREP,(string, ATOM, ATOM));
IMPORT string NEWATOM;
PROTOTYPE(void STARTATOM,(void));
PROTOTYPE(void ATOMCHAR,(char ));
PROTOTYPE(void DOOP,(void));
PROTOTYPE(void DOPRIVATE,(void));
PROTOTYPE(void DOHIDE,(void));
PROTOTYPE(boolean DONAME,(void));
PROTOTYPE(void Init_Atoms,(void ));
#if USER
PROTOTYPE(void InitUAtom,(ATOM,string,OCLASS,int ,CLASS,boolean));
#endif

/*
** datab.c
*/
IMPORT CLAUSE ANDG;
IMPORT CLAUSE OR1G;
IMPORT CLAUSE OR2G;
IMPORT CLAUSE IMPG;
IMPORT CLAUSE DUMMYCL;
PROTOTYPE(void Init_Skeleton,(void));
PROTOTYPE(int  Var_Count,(void));
PROTOTYPE(TERM SKELETON,(TERM,int ,int));
PROTOTYPE(CLAUSE NewClause,(TERM,TERM,boolean));
PROTOTYPE(TERM ClauseHead,(CLAUSE,TERM));
PROTOTYPE(TERM ClauseBody,(CLAUSE,TERM));
PROTOTYPE(void freeterm,(int,TERM,int));
PROTOTYPE(void FreeClause,(CLAUSE));

PROTOTYPE(void DOASSERT,(boolean));
PROTOTYPE(boolean DOCONSULT,(TERM,boolean,boolean));
PROTOTYPE(boolean DOENSURE,(void));
PROTOTYPE(void DOABOLISH,(int));
PROTOTYPE(boolean DOCLAUSE,(void));
PROTOTYPE(boolean DORETRACT,(boolean));

PROTOTYPE(void retractclauses,(void));

PROTOTYPE(void Init_Datab,(void));

/*
** eval.c
*/
IMPORT TERM A0;
IMPORT TERM A1;
IMPORT TERM A2;
IMPORT TERM BCT;
IMPORT TERM CALLX;
IMPORT ENV CALLXENV;
PROTOTYPE(boolean isatom,(TERM));
PROTOTYPE(void CHECKATOM,(TERM));
PROTOTYPE(boolean DOEQUAL,(TERM,TERM,int ));
PROTOTYPE(boolean ground,(TERM,int));
PROTOTYPE(int compare,(TERM,TERM,int));
PROTOTYPE(boolean CALLEVALPRED,(TERM,ENV));

/*
** exec.c
*/
IMPORT ENV CHOICEPOINT;
IMPORT int SPYCOUNT;
PROTOTYPE(boolean EXECUTE,(TERM,ENV));

/*
** io.c
*/
IMPORT boolean FERRORFLAG;
IMPORT int IOERRORFLAG;
PROTOTYPE(TERM phy_name,(ATOM)); 
PROTOTYPE(void wc,(char));
PROTOTYPE(void ws,(string));
PROTOTYPE(void out ,(int , char *));
IMPORT int ERRPOS;
PROTOTYPE(void FileError,(ERRORTYPE));
PROTOTYPE(boolean FILEENDED,(void));
PROTOTYPE(boolean LINEENDED,(void));
IMPORT char CH;
IMPORT char LASTCH;
PROTOTYPE(void GETCHAR,(void));
PROTOTYPE(void REGET,(void));
PROTOTYPE(void ABORT,(ERRORTYPE));
PROTOTYPE(void ERROR,(ERRORTYPE));
PROTOTYPE(void SYNERROR,(ERRORTYPE));
PROTOTYPE(void ARGERROR,(void));
PROTOTYPE(void SYSTEMERROR,(string));
PROTOTYPE(void ERROR_MESS,(ERRORTYPE));
PROTOTYPE(void START_WARNING,(void));
PROTOTYPE(void END_WARNING,(void));

PROTOTYPE(boolean DOSEE,(void));
PROTOTYPE(boolean DOOPEN,(void));
PROTOTYPE(boolean DOCLOSE,(void));
PROTOTYPE(boolean DOTELL,(void));
PROTOTYPE(boolean DOSEEK,(void));
PROTOTYPE(boolean DOGET,(void));
PROTOTYPE(boolean DOGET0,(void));
PROTOTYPE(boolean DOASK,(void));
PROTOTYPE(boolean DOFASSIGN,(void));
PROTOTYPE(void DOSKIP,(void));
PROTOTYPE(void DOTAB,(void));
PROTOTYPE(void DOPUT,(void));
PROTOTYPE(void DOCLS,(void));
PROTOTYPE(boolean DOGOTOXY,(void));


PROTOTYPE(void getinfile,(void));
PROTOTYPE(void setinfile,(void));
PROTOTYPE(void getoutfile,(void));
PROTOTYPE(void setoutfile,(void));

PROTOTYPE(string itoa,(int));
PROTOTYPE(string ltoa,(LONG));
#if REALARITH
PROTOTYPE(string ftoa,(REAL));
#endif

PROTOTYPE(ATOM error_string, (ERRORTYPE));

PROTOTYPE(void Init_Io,(void));
PROTOTYPE(boolean DOWINDOW,());

/*
** misc.c
*/
PROTOTYPE(void print_copyright,(void));
PROTOTYPE(boolean DOTIME,(ATOM));
PROTOTYPE(boolean DOTIMER,(void));
PROTOTYPE(boolean islist,(TERM,boolean,boolean));
PROTOTYPE(int get_num_env_val,(char *));

/*
** prolog.c
*/

IMPORT string PROTFILE;
IMPORT int ARGC;
IMPORT char **ARGV;
IMPORT ENV E;
IMPORT TERM BE;
IMPORT boolean HALTFLAG;
IMPORT boolean TRACING;
IMPORT boolean SPYTRACE;
IMPORT boolean ECHOFLAG;
IMPORT boolean DEBUGFLAG;
IMPORT boolean PROTFLAG;
IMPORT boolean OCHECK;
IMPORT boolean xWINDOW_ON;
IMPORT boolean aSYSMODE;
IMPORT boolean VERBOSEFLAG;
IMPORT boolean USER_UNIFY_FLAG;
IMPORT boolean In_Toplevel_Read;
IMPORT boolean UserAbort;

IMPORT ERRORTYPE ERRORFLAG;

IMPORT jmp_buf error_label;

PROTOTYPE(void ERROR_JMP,(void));
PROTOTYPE(void ABORT_JMP,(void));
PROTOTYPE(void EXIT_JMP,(int));
PROTOTYPE(void SAVE_ERROR,(char *));
PROTOTYPE(void RESTORE_ERROR,(char *));
#define CATCH_ERROR() (setjmp(error_label),ERRORFLAG!=NOERROR)

/*
** read.c
*/

PROTOTYPE(boolean DOREAD,(void));
PROTOTYPE(TERM READIN,(void));  
PROTOTYPE(int VARCOUNT,(void));  
PROTOTYPE(TERM VARTERM,(void));
PROTOTYPE(int LPREC,(ATOM));
PROTOTYPE(int RPREC,(ATOM));

/*
** sys.c
*/
PROTOTYPE(void Init_Sys,(void));
PROTOTYPE(int call_system,(char *));
PROTOTYPE(long TIMER,(void));
PROTOTYPE(int file_exist,(char *));
PROTOTYPE(char *t_cls,(void));
PROTOTYPE(char *t_gotoxy,(int,int));
PROTOTYPE(int t_lines,(void));
PROTOTYPE(int t_columns,(void));
PROTOTYPE(char *setenv,(char *));

/*
** types.c in types.h
*/
PROTOTYPE(void DOSTATS,(void));
PROTOTYPE(void Init_Types,(void));
PROTOTYPE(void collect_garbage,(boolean));
#if USER
PROTOTYPE(void reserve_atom,(ATOM));
#endif
#if SAVE
PROTOTYPE(ERRORTYPE save_restore,(boolean, string));
#endif

/*
** uni.c
*/
PROTOTYPE(boolean INTRES,(TERM,int));
PROTOTYPE(boolean LONGRES,(TERM,LONG));
PROTOTYPE(boolean UNIFY,(int,TERM,TERM,TERM,TERM,int));

#if USER
/*
** user.c
*/
PROTOTYPE(void Init_User,(void));
PROTOTYPE(boolean CallUser,(TERM,ENV));
#endif
/*
** version.c
*/
IMPORT string version;
IMPORT long MAGIC_NUMBER;

/*
** write.c
*/
PROTOTYPE(void wq,(ATOM));
PROTOTYPE(void DISPLAY,(TERM));
PROTOTYPE(void LISTOUT,(TERM));
PROTOTYPE(void WRITEOUT,(TERM,boolean));
PROTOTYPE(void ABORT_WRITE,(TERM));
IMPORT ENV TRACE_GOON;
PROTOTYPE(boolean TRACE,(ATOM,TERM,ENV));

/*
** charcl.c
*/
IMPORT unsigned char _CHARCLASS[256];

/*
** help.c
*/

#if HELP
PROTOTYPE(boolean DOHELP, (ATOM));
#endif

#endif

/* end of file */
