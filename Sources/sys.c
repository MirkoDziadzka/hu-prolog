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


#include "config.h"

extern char * getenv();
extern int DEBUGFLAG;


/*GLOBAL*/ int get_num_env_val( var )
    char *var;
{
    char *value;
    int Result = 0;

    if((value = getenv(var)) == (char *)0)
	return -1;
    for( Result = 0; *value && *value >= '0' && *value <= '9' ; ++value)
    {
	Result *= 10;
	Result += *value - '0';
    }

    if(*value)
	return -2;

    return Result;
}
    
#if UNIX
#if POSIX
# include <limits.h>
# include <unistd.h>
#else
# if SYSV
#  include <sys/param.h>
# elif BSD
#  include <sys/types.h>
#  include <sys/acct.h>
#  include <sys/param.h>
#  if defined(AHZ) && !defined(HZ)
#   define HZ AHZ
#  endif
# endif
#endif

#ifndef HZ
# if 0
    you may use the following definition and hope, that
    the HZ will be defined in the Environment at runtime
# else
#	define HZ 100 
# endif
#endif

static int HZ_VAL;
static void init_HZ()
{
    static int is_init = 0;
    if(is_init)
	return;
    is_init = 1;
#if POSIX
    if((HZ_VAL = sysconf(_SC_CLK_TCK)) <= 0)
	HZ_VAL = HZ;
#if DEBUG
    else if(DEBUGFLAG)
    {
	out(2,"sysconf(_SC_CLK_TCK) == ");
	out(2,itoa(HZ_VAL));
	out(2,"\n");
    }
#endif
#else
    if((HZ_VAL = get_num_env_val("HZ")) <= 0)
	HZ_VAL = HZ;
#if DEBUG
    else if(DEBUGFLAG)
    {
	out(2,"getenv(HZ) == ");
	out(2,itoa(HZ_VAL));
	out(2,"\n");
    }
#endif
#endif
#if DEBUG
    if(DEBUGFLAG)
    {
	out(2,"HZ == ");
	out(2,itoa(HZ_VAL));
	out(2,"\n");
    }
#endif
}
#endif
#if MS_DOS
# include <time.h>
# ifndef CLK_TCK
-------> CLK_TCK should be defined here
# endif
#endif

long TIMER()
{
#if UNIX  || VMS
    static struct {
        long usertime,systime,cusertime,csystime;
    } UTIME;
    extern long times(); /* from clib */
    (void)times(&UTIME);
    return (UTIME.usertime * 100) / HZ_VAL;
#endif

#if MS_DOS
    extern long clock(); /* from clib */
    return (long)((clock()  * 100) / CLK_TCK);
#endif
#if !(UNIX || VMS || MS_DOS)
    return 0L;
#endif
}


int file_exist(filename)
char *filename;
{
    extern int access();
    return access(filename,0x04)==0 ;
}

/*
** call_system
*/
int call_system(command)
    char *command;
{
    int system();       /* from clib */
    int result;
    result =  system(command);
#if UNIX || MS_DOS
    return result == 0;
#endif
#if VMS
    return result != 0;
#endif
}

/* setenv.c */
#define ENVSIZE           32   /* max. number of entrys */
#define CHARSIZE        1024   /* character space  */
#ifndef NULL
#define NULL      ((char *)0)
#endif

extern char **environ;
extern int  strncmp();

static char *new_env[ENVSIZE+1];
static char  new_char[CHARSIZE];


char *setenv(env_string)
register char env_string[];
{
    register char *s;
    register char *first_free; /* first free character */
    register char *last_free;  /* last free character */
    register int i=0;
    int l1;                    /* position of '=' in env_string */
    char **envp=environ;       /* temporary env-pointer */

    for(l1=0,s=env_string;*s;++s)
        if(*s == '='){
            l1 = s-env_string;
            break;
        }
    if(!*s) return NULL;

    first_free = new_char;
    last_free  = new_char + CHARSIZE -1;
    while((s = *envp) != (char *)0)
    {
        if(strncmp(s,env_string,l1) != 0)
        {
            new_env[i++]  = first_free;
            while(first_free < last_free && (*first_free++ = *s++) != '\0');
            if(first_free >= last_free || i >= ENVSIZE)
            { 
                new_env[--i] = NULL; 
                return NULL;
            }
        }
        ++envp;
    }
    s=env_string;
    new_env[i++] = first_free;
    while(first_free < last_free && (*first_free++ = *s++) != '\0');
    if(first_free >= last_free || i >= ENVSIZE)
    { 
        new_env[--i] = NULL; 
        return NULL;
    }

    new_env[i] = NULL;
    environ = new_env;
    return new_env[i-1];
}

/*
** terminal
*/
static int LINES = 25;
static int COLUMNS = 80;

#if UNIX 
#ifndef NO_TERMCAP
extern int tgetent();
extern int tgetnum();
extern char *tgetstr();
extern char *tgoto();
extern tputs();
#endif

#ifndef NULL
#	define NULL	(char *)0
#endif

static char *CM = NULL;
static char *CL = NULL;
static char *CR = NULL;


static void init_terminal()
{
#ifndef NO_TERMCAP
    char tbuffer[1024];
    static char xbuffer[128]; /* STATIC !!!! */
    char *area = xbuffer;

    /* termcap lesen */
    if(tgetent(tbuffer,getenv("TERM")) == 1)
    {
	 CM = tgetstr("cm",&area);
	 CL = tgetstr("cl",&area);
	 CR = tgetstr("cr",&area);
	 LINES = tgetnum("li");
	 COLUMNS = tgetnum("co");
    }
#endif
    /* environment lesen */
    if(get_num_env_val("LINES") > 0)
	LINES = get_num_env_val("LINES");
    if(get_num_env_val("COLUMNS") > 0)
	COLUMNS = get_num_env_val("COLUMNS");
    /* default Werte */
    if(LINES <= 0)
        LINES = 24;
    if(COLUMNS <= 0)
        COLUMNS = 80;
    if(CL == NULL)
	CL = "";
    if(CM == NULL)
	CM = "";
}
#endif

#define BUFLEN	128
static char buffer[BUFLEN];
static int b_index;

#define  reset_buffer()	b_index = 0
#define  end_buffer()	buffer[b_index] = '\0'
static int put_char(c)
    char c;
{
    buffer[b_index++] = c;
    if(b_index >= BUFLEN)
        --b_index;
	return 0;
}

/*GLOBAL*/ char *t_cls()
{
#if UNIX
#ifndef NO_TERMCAP
    reset_buffer();
    tputs(CL,LINES,put_char);
    end_buffer();
    return buffer;
#endif
#ifdef NO_TERMCAP
    return "";
#endif
#endif
#if MS_DOS
    static char CL[] = "\033[2J\033[0;0H";
    return CL;
#endif
}

/*GLOBAL*/ char *t_gotoxy(col,row)
    int col,row;
{
#if UNIX
#ifndef NO_TERMCAP
    reset_buffer();
    tputs(tgoto(CM,col-1,row-1),LINES,put_char);
    end_buffer();
    return buffer;
    
#endif
#ifdef NO_TRRMCAP
    return "";
#endif
#endif
#if MS_DOS
    static char CM[] = "\033[yy;xxH";
    CM[2] = (char)((row / 10) + '0');
    CM[3] = (char)((row % 10) + '0');
    CM[5] = (char)((col / 10) + '0');
    CM[6] = (char)((col % 10) + '0');
    return CM;
#endif
}

/*GLOBAL*/ int t_columns()
{
    return COLUMNS;
}

/*GLOBAL*/ int t_lines()
{
#if WINDOWS
    extern int xWINDOW_ON;

    if(xWINDOW_ON)
	return LINES -1;
    else
#endif
    return LINES;
}


void Init_Sys()
{
#if UNIX
    init_HZ();
    init_terminal();
#endif
}

/* end of file */
