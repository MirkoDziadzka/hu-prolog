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

#include <signal.h>
#include <string.h>

#include "config.h"
#include "types.h"
#include "atoms.h"
#include "files.h"
#include "memory.h"
#include "extern.h"

#include "paths.h"


extern void add_string_to_argv(int *argc, char ***argv, char * envargs);

/* Variable declarations */

LOCAL string PROLIB= (string) 0;
LOCAL string PROG  = (string) 0;
LOCAL string RESTORE  = (string) 0;
GLOBAL string PROTFILE = (string) 0;

GLOBAL int ARGC;
GLOBAL char **ARGV;

GLOBAL ENV E;
GLOBAL TERM BE;
GLOBAL boolean HALTFLAG         = false;
GLOBAL boolean TRACING          = false; 
GLOBAL boolean SPYTRACE         = false;
GLOBAL boolean ECHOFLAG         = false;
GLOBAL boolean DEBUGFLAG        = false;
GLOBAL boolean PROTFLAG         = false;
GLOBAL boolean OCHECK           = false;
GLOBAL boolean xWINDOW_ON       = false;
GLOBAL boolean aSYSMODE         = false;
GLOBAL boolean VERBOSEFLAG      = true;
GLOBAL boolean USER_UNIFY_FLAG	= false;
GLOBAL boolean In_Toplevel_Read = false;
LOCAL  boolean READ_STD_PLGRC	= true;
GLOBAL card WARNFLAG = WARN_ON;


GLOBAL boolean UserAbort;
GLOBAL ERRORTYPE ERRORFLAG;
LOCAL  int     ReturnCode;

GLOBAL jmp_buf error_label;
LOCAL  jmp_buf abort_label;
LOCAL  jmp_buf exit_label;


GLOBAL VOLATILE void ERROR_JMP()
{
    longjmp(error_label,1);
}

GLOBAL VOLATILE void ABORT_JMP()
{
    longjmp(abort_label,1);
}

GLOBAL VOLATILE void EXIT_JMP(N)
    int N;
{
    ReturnCode = N;
    longjmp(exit_label,1);
}

GLOBAL void SAVE_ERROR(adr)
    char *adr;
{
    (void)memcpy(adr,(char *)error_label,sizeof(jmp_buf));
}

GLOBAL void RESTORE_ERROR(adr)
    char *adr;
{
    (void)memcpy((char *)error_label,adr,sizeof(jmp_buf));
}

#if SIGNAL_RETURNS_VOID
LOCAL void sig_handler(s)
    int s;
#endif
#if ! SIGNAL_RETURNS_VOID
LOCAL int sig_handler(s)
    int s;
#endif
{
    (void)signal(s,SIG_IGN);
    switch(s)
    {
    case SIGINT:
        UserAbort= true;
#if WINDOWS
        if(xWINDOW_ON)
            w_scr_refresh();
#endif
        (void)signal(SIGINT,sig_handler);
        break;
#ifdef SIGQUIT
    case SIGQUIT: 
        EXIT_JMP(-1);
#endif
#ifdef SIGBREAK
    case SIGBREAK: 
        EXIT_JMP(-1);
#endif
#ifdef SIGPIPE
    case SIGPIPE:
	(void)signal(SIGPIPE,SIG_IGN);
	EXIT_JMP(-1);
#endif
#ifdef SIGFPE
    case SIGFPE:
	out(2,"-- signal FPE received - what's wrong ??\n");
	(void)signal(SIGFPE,sig_handler);
	ABORT(FPEE);
	/*NOTREACHED*/
#endif
    default:
#if UNIX
        out(2,"\nsignal ");out(2,itoa(s));out(2," received\n");
        (void)signal(s,SIG_DFL);
#if WINDOWS
        w_exit();
#endif
	SYSTEMERROR("unexpected signal received");
	/*
        (void)kill(getpid(),s);
        (void)pause();
	*/
	exit(-1);
#endif
        (void)signal(s,sig_handler);
    }
#if !SIGNAL_RETURNS_VOID
    return 0;
#endif
}

#if MSC
LOCAL void (interrupt far * old_1b)();
LOCAL int _1b_is_init = 0;
LOCAL void interrupt far new_1b()
{ UserAbort = 1; }

LOCAL void init_1b()
{
        if(! _1b_is_init)
        {
            old_1b = _dos_getvect(0x1b);
                _dos_setvect(0x1b,new_1b);
                _1b_is_init = 1;
        }
}

LOCAL void exit_1b()
{
        if(_1b_is_init)
        {
                _dos_setvect(0x1b,old_1b);
                _1b_is_init = 0;
        }
}
#endif
#if TURBOC
LOCAL void interrupt  (* old_1b)();
LOCAL int _1b_is_init = 0;
LOCAL void interrupt new_1b()
{ UserAbort = 1; }

LOCAL void init_1b()
{
    if(! _1b_is_init)
    {
        old_1b = getvect(0x1b);
        setvect(0x1b,new_1b);
        _1b_is_init = 1;
    }
}

LOCAL void exit_1b()
{
        if(_1b_is_init)
        {
                setvect(0x1b,old_1b);
                _1b_is_init = 0;
        }
}
#endif

#define CATCH( sig ) 	if(signal(sig,sig_handler) == SIG_IGN) \
				(void)signal(SIGINT,SIG_IGN) ; \
			else  /* nothing */
void Init_Signals()
{
#ifdef SIGHUP
    CATCH( SIGHUP );
#endif
#ifdef SIGINT
    CATCH( SIGINT );
#endif
#ifdef SIGQUIT
    CATCH( SIGQUIT );
#endif
#ifdef SIGILL
    CATCH( SIGILL );
#endif
#ifdef SIGTRAP
    CATCH( SIGTRAP );
#endif
#ifdef SIGIOT
    CATCH( SIGIOT );
#endif
#ifdef SIGABRT
    CATCH( SIGABRT );
#endif
#ifdef SIGEMT
    CATCH( SIGEMT );
#endif
#ifdef SIGFPE
    CATCH( SIGFPE );
#endif
#ifdef SIGBUS
    CATCH( SIGBUS );
#endif
#ifdef SIGSEGV
    CATCH( SIGSEGV );
#endif
#ifdef SIGSYS
    CATCH( SIGSYS );
#endif
#ifdef SIGPIPE
    CATCH( SIGPIPE );
#endif
#ifdef SIGALRM
    CATCH( SIGALRM );
#endif
#ifdef SIGTERM
    CATCH( SIGTERM );
#endif

#if MS_DOS
    init_1b();
#endif
}

LOCAL TERM make_toplevel(X)
    TERM X;
{ 
    TERM Y;
    if (name(X)==COMMA_2)
    {
        X=son(X);
        return mk2sons(name(X),son(X),GOTOT, make_toplevel(br(X)));
    }
    else if ((Y=VARTERM()) != NIL_TERM)
        return mk2sons(name(X),son(X),GOTOT,Y);
    else 
        return mk2sons(name(X),son(X),NIL_ATOM,NIL_TERM);
}


LOCAL void TOPLEVEL ()
{ 
    TERM  X;
    CLAUSE MCL;
    static int phase = 0;
    boolean print_prompt = true;

    HALTFLAG= false;
    for(;;)
    {
        init_string_stack();
        init_atom_stack();
        init_term_stack();
        init_trail_stack();
        init_env_stack();

	CALLX = NIL_TERM;

        if((inputfile=OpenFile(phy_name(STDIN_0),read_mode)) < FIRSTFILE)
            inputfile=STDIN;
        FLOGNAME(inputfile) = STDIN_0;

        if(!ISINPUT(inputfile))
            LINELENGTH = CHARPOS = 0;
        ISINPUT(inputfile) = true;

        if((outputfile = OpenFile(phy_name(STDOUT_0),write_mode)) < FIRSTFILE)
            outputfile=STDOUT;
        FLOGNAME(outputfile) = STDOUT_0;

        retractclauses();

        CHOICEPOINT=E=newenv();
        BE=base(E)=NIL_TERM;

        switch(phase)
        {
        case 0:
            ++phase;
            if(clause(LOGIN_0) != NIL_CLAUSE)
                (void)EXECUTE(mkatom(LOGIN_0),NIL_ENV);
            else if(VERBOSEFLAG)
	    {
		if(xWINDOW_ON && outputfile != STDOUT)
		    print_copyright();
                ws("\nType `help.' for help\n");
                ws("\nReady\n");
	    }
            break;
        case 1:
            if(HALTFLAG)
            {
                ++phase;
                if(clause(LOGOUT_0) != NIL_CLAUSE)
                    (void)EXECUTE(mkatom(LOGOUT_0),NIL_ENV);
                EXIT_JMP(0);
            }
            else if(clause(TOP_0) != NIL_CLAUSE) 
            {
                (void)EXECUTE(mkatom(TOP_0),NIL_ENV);
                print_prompt = true;
            }
            else if(print_prompt)
            {
                boolean tmp_trace = SPYTRACE;
                print_prompt = false;
                if(! DEBUGFLAG)
                    SPYTRACE = false;
                if(clause(PROMPT_0) != NIL_CLAUSE)
                    (void)EXECUTE(mkatom(PROMPT_0),NIL_ENV);
                else if(VERBOSEFLAG)
                    ws("\n?-");
                SPYTRACE = tmp_trace;
            }
            else 
            { 
                In_Toplevel_Read = true;
                X= READIN();
                In_Toplevel_Read = false;
                clause(MAIN_0)= MCL= stackterm(3+1); /*@*/
                nextcl(MCL)=NIL_CLAUSE; 
                setnvars(MCL,VARCOUNT());
		set_in_use(MCL);
                name(head(MCL))=MAIN_0; 
                son(head(MCL))=NIL_TERM;
                name(body(MCL))=GOTOT; 
                son(body(MCL))=make_toplevel(X);
                if(EXECUTE(mkatom(MAIN_0),NIL_ENV))
                {
                    if(!HALTFLAG && VERBOSEFLAG)
                        ws("\nyes");
                }
                else if(VERBOSEFLAG)ws("\nno");
                print_prompt = true;
            }
            break;
        default:
            EXIT_JMP(1);
        }
        IOERRORFLAG=0;
        if (UserAbort) ABORT(ABORTE);
    }  
}

LOCAL void InitArg(argc,argv)
    int argc;
    char *argv[];
{
    int i;
    ARGC = argc;
    ARGV = argv;

    for(i=1;i<argc;i++)
    {
        if(*argv[i]!='-') 
        {
            PROG = argv[i];
            continue;
        }
        switch(argv[i][1])
        { /* option */
        case 'd' :
        case 'D' : DEBUGFLAG = true; break;
        case 'L' : PROTFLAG = true;
        case 'l' : PROTFILE = argv[++i]; break;
        case 'S' : READ_STD_PLGRC = false; /* no break */
        case 's' : if(argv[++i] && *argv[i]) 
		       PROLIB = argv[i]; 
		   else 
		       PROLIB = "\0";
		   break;
        case 'w' : 
        case 'W' : WARNFLAG = WARN_OFF; break;
        case 'v' : 
        case 'V' : VERBOSEFLAG = false; break;
#if OCCUR_CHECK
        case 'o' : 
        case 'O' : OCHECK = !OCHECK; break;
#endif
#if WINDOWS
        case 'x' : 
        case 'X' : xWINDOW_ON = !xWINDOW_ON; break;
#endif
	case 't' :
	case 'T' : SPYTRACE = TRACING = true; break;
        case 'r' : 
        case 'R' : if(argv[++i] && *argv[i])
		       RESTORE = argv[i]; 
		   else 
		       RESTORE = "\0";
		   break;
        }
    } /* for */
}

#if UNIX
LOCAL string PLGRCNAME=".prologrc";
#define SEP_CHAR        '/'
#endif
#if MS_DOS 
LOCAL string PLGRCNAME="prolog.rc";
#define SEP_CHAR        '\\'
#endif
#if VMS 
LOCAL string PLGRCNAME="prolog.rc";
#endif

LOCAL void prologrcfile()
{
    string S,A,I;
    static char AA[100];
    A="";
    if(file_exist(PLGRCNAME) != 0) 
        A= PLGRCNAME;
#if UNIX || MS_DOS
    else
    { 
        A= AA;
        S=getenv("HOME");
        I= A; 
        while (*S) 
            *I++ = *S++;
        if(*(--S) != SEP_CHAR)
            *I++ = SEP_CHAR;
        S=PLGRCNAME;
        while (*S) 
            *I++ = *S++;
        *I= '\0';
        if(file_exist(A) == 0) 
            return ;
    }
#endif
    if(*A) 
        PROLIB = A;
    return ;
}

int main(argc,argv)
    int argc; 
    char **argv;
{
    static boolean first_time = true;
    static boolean CANT_RESTORE = false;

    ERRORFLAG = NOERROR;

#if USE_HUPROLOG_ENV_VAR
    add_string_to_argv(&argc, &argv, getenv("HUPROLOG"));
#endif

    if(setjmp(exit_label) != 0)
        goto exit_main;

    Init_Signals();

    InitArg(argc,argv); /* before Init_Windows */

    Init_Sys();
    Init_Types();
    Init_Atoms();
    Init_Arith();
    Init_Datab();

#if USER
    Init_User();
#endif

#if SAVE
    if (RESTORE && *RESTORE)
    {
	if((ERRORFLAG = save_restore(false,RESTORE)) != NOERROR)
	    first_time = false;
	else
	    CANT_RESTORE = true;
    }
#endif

#if WINDOWS
    if(xWINDOW_ON)
        Init_Windows();
#endif
#if !WINDOWS
    xWINDOW_ON = false;
#endif

    Init_Io();

    if(VERBOSEFLAG)
	print_copyright();
    if(ERRORFLAG != NOERROR)
    {
	CALLX = NIL_TERM;
	ERROR_MESS(ERRORFLAG);
    }

    ReturnCode = 0;
    if(setjmp(exit_label) != 0)
        goto exit_main;
    (void)setjmp(abort_label); 
    Init_Io();
    ERRORFLAG = NOERROR;
    if(CATCH_ERROR())
        ABORT(ERRORFLAG);
    UserAbort= false;

    init_string_stack();
    init_atom_stack();
    init_term_stack();
    init_trail_stack();
    init_env_stack();

    if(first_time && !CANT_RESTORE)
    {
	if(!PROLIB)
	    prologrcfile();

        first_time = false;
        CHOICEPOINT=E=newenv();
        BE=base(E)=NIL_TERM;

	aSYSMODE = true;
#ifdef STDPLGRC
	if(READ_STD_PLGRC && file_exist(STDPLGRC) != 0)
        {
            if((inputfile=OpenFile(phy_name(STDIN_0),read_mode)) < FIRSTFILE)
                inputfile=STDIN;
            FLOGNAME(inputfile) = STDIN_0;
            if((outputfile=OpenFile(phy_name(STDOUT_0),write_mode)) < FIRSTFILE)
                outputfile=STDOUT;
            FLOGNAME(outputfile) = STDOUT_0;
            A0 = mkatom(LOOKUP(STDPLGRC,0,false,true));
            (void)DOCONSULT(A0,false,true);
        }
#endif
        aSYSMODE = true;
        if (PROLIB && *PROLIB)
        {
            if((inputfile=OpenFile(phy_name(STDIN_0),read_mode)) < FIRSTFILE)
                inputfile=STDIN;
            FLOGNAME(inputfile) = STDIN_0;
            if((outputfile=OpenFile(phy_name(STDOUT_0),write_mode)) < FIRSTFILE)
                outputfile=STDOUT;
            FLOGNAME(outputfile) = STDOUT_0;
            A0 = mkatom(LOOKUP(PROLIB,0,false,true));
            (void)DOCONSULT(A0,false,true);
        }
        aSYSMODE = false;
	if((inputfile=OpenFile(phy_name(STDIN_0),read_mode)) < FIRSTFILE)
	    inputfile=STDIN;
	FLOGNAME(inputfile) = STDIN_0;
	if((outputfile=OpenFile(phy_name(STDOUT_0),write_mode)) < FIRSTFILE)
	    outputfile=STDOUT;
	FLOGNAME(outputfile) = STDOUT_0;
#if WINDOWS && 0
	if(VERBOSEFLAG &&
	   FTYPE(outputfile) == WINDOWFT && outputfile != STDOUT)
	{
	    print_copyright();
	}
#endif
        if(PROG && *PROG)
        {
            A0 = mkatom(LOOKUP(PROG,0,false,true));
            (void)DOCONSULT(A0,false,false);
        }
    }
    first_time = false;
    aSYSMODE = false;
    TOPLEVEL();
  exit_main:
    outputfile = STDOUT;
#if WINDOWS
    if(xWINDOW_ON)
        w_exit();
#endif
#if MS_DOS
    exit_1b();
#endif
    exit(ReturnCode);
    /*NOTREACED*/
    return ReturnCode; 
}


/* end of file */
