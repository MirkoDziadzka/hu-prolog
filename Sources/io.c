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


#include <string.h>

#include "config.h"
#include "types.h"
#include "atoms.h"
#include "files.h"
#include "extern.h"

LOCAL TERM tempterm = NIL_TERM;  /* for phy_name */

/*********************************************************/
/*                filedefinitions                        */
/*********************************************************/


GLOBAL file_type file_tab[MAXFILES];
GLOBAL boolean FERRORFLAG=true;
GLOBAL file inputfile,outputfile;

GLOBAL TERM phy_name(A)   
    ATOM A;
{
    int depth = 0;

    TERM F;
    CLAUSE CL;
start:;
    if(++depth > 100)
        ERROR(DEPTHE);
    CL = clause(FNAME_2);
    while(CL != NIL_CLAUSE)
    {
        if(nvars(CL) != 0) 
            SYSTEMERROR("phy_filename");
        if(name(br(F = son(head(CL)))) == A)
        { 
            if(isatom(F))
            { 
                A=name(F) ; 
                goto start; 
            }
            return F;
        }
        else CL = nextcl(CL);
    }
    name(tempterm) = A;
    return tempterm;
}

GLOBAL file OpenFile(filename , mode)
    TERM filename;
    fmode mode;
{
    file f;

    deref(filename);
    if(! ground(filename,MAXDEPTH)) 
        SYSTEMERROR("OpenFiles.0");
    /* look for filename in filetab */
    for(f=FIRSTFILE;f <= LASTFILE;++f)
    { 
        if(FNAME(f)==NIL_TERM || !UNI(filename,FNAME(f))) 
            continue;
#if WINDOWS
        if(!xWINDOW_ON)
#endif
            return f;
#if WINDOWS
        if(FTYPE(f) == NORMFT) 
            return f;
        if(FTYPE(f) == WINDOWFT)
        { 
            if(mode != look_mode) 
                w_up(FWINPTR(f)); 
            return f; 
        }
        SYSTEMERROR("OpenFiles.1");
#endif
    }
    /* file not open */
    for(f=FIRSTFILE;f <= LASTFILE && FNAME(f) != NIL_TERM; f++)
    if(f > LASTFILE) 
        return ERRFILE;
    if(isatom(filename)) /* an ordinary file */
    { 
        string fn;
        fn = tempstring(atomstring(name(filename)));
        switch(mode)
        {
        case look_mode: 
            return NOFILE;
        case read_mode:
            if((FINPTR(f)=open(fn,0))<0)
	    {
                return NOFILE;
	    }
            CANREAD(f)=!(CANWRITE(f)=false);
            FOUTPTR(f) = FINPTR(f);
            break;
        case write_mode:
            if((FOUTPTR(f)=creat(fn,0666))<0)
                return NOFILE;
            CANREAD(f)=!(CANWRITE(f)=true);
            FINPTR(f) = FOUTPTR(f);
            break;
        case read_write:
            if((FOUTPTR(f)=FINPTR(f)=open(fn,2))<0)
            {
                if(file_exist(fn) != 0) 
                    return NOFILE;
                (void)close(creat(fn,0666));
                if((FINPTR(f)=FOUTPTR(f)=open(fn,2))<0)
                    return NOFILE;
            }
            CANREAD(f)=CANWRITE(f)=true;
            break;
        default:
            SYSTEMERROR("OpenFile.2");
        } /* switch */
        FNAME(f) = SKELETON(filename,1,MAXDEPTH); 
        FTYPE(f) = NORMFT;
        EOF(f)=ISINPUT(f)=false;
        ISTTY(f)=isatty(FINPTR(f));
        FLINENO(f) = 0;
        FCHARPOS(f) = FBUFLENGTH(f) = 0;
	FUNGET(f) = false;
        return f;
    }
#if WINDOWS
    else if(xWINDOW_ON && name(filename) == WINDOW_6)
    {
        int a,b,c,d;
        static string winname;
	int win_mode = NORMAL;
        TERM T;

        if(mode == look_mode) return NOFILE;
        a=INTVALUE(arg(1,filename));
        b=INTVALUE(arg(2,filename));
        c=INTVALUE(arg(3,filename));
        d=INTVALUE(arg(4,filename));

	T=arg(5,filename);
        CHECKATOM(T);
        winname= tempstring(atomstring(copyatom(name(T))));


	T=arg(6,filename);
	if(! islist(T,false,false))
	    ERROR(ARGE);
	for( ; name(T) == CONS_2 ; T = arg2(T))
	{

	    switch(name(arg1(T)))
	    {
		case PAGING_0: win_mode |= PAGING; break;
		default:       /* ignore unknown values */;
	    }
	}
        FTYPE(f) = WINDOWFT;
        if((FWINPTR(f) = w_create(a,b,c,d,winname,win_mode))==NOWINDOW)
            return ERRFILE;
        FNAME(f) = SKELETON(filename,1,MAXDEPTH);
        EOF(f)=ISINPUT(f)=false;
        ISTTY(f)=true;
        CANREAD(f)=CANWRITE(f)=true;
        FCHARPOS(f) = FBUFLENGTH(f) = 0;
	FUNGET(f) = false;
        return f;
    }
#endif
    else return NOFILE;
}

GLOBAL void CloseFile(f)
    file f;
{
    if(f<=STDTRACE || FNAME(f) == NIL_TERM) 
        return;
    if(FTYPE(f)== NORMFT)
    {
        if(FINPTR(f) > 2) (void)close(FINPTR(f));
        if(FOUTPTR(f) > 2) (void)close(FOUTPTR(f));
    }
#if WINDOWS
    else if(xWINDOW_ON && FTYPE(f)==WINDOWFT) 
        w_remove(FWINPTR(f));
#endif
    else SYSTEMERROR("CloseFile.1");
    FNAME(f)=NIL_TERM;
}

/******************************************************/
/*                     basicio                        */
/******************************************************/

#define MAXDIGITS 30   /* max digits for number conversation */
LOCAL  char numbuffer[MAXDIGITS]; /* for number conversation */


LOCAL int PRTF = -1; /* os - filepointer to protfile */
GLOBAL int IOERRORFLAG=0;

LOCAL void IOError(f)
TERM  f;
{ 
    FORWARD void ws();
    if(IOERRORFLAG++>3) 
        ABORT(IOERROR);
    if((outputfile=OpenFile(phy_name(STDERR_0),write_mode))<FIRSTFILE)
        ABORT(IOERROR);
    ws("\ni/o error in file '"); 
    DISPLAY(f); 
    ERROR(IOERROR);
}

#if REALARITH

GLOBAL string ftoa(d)
    double d;
{
    string cp;
    int r;
    int expo ;

    cp = numbuffer;
    if (d != 0.0 && d == 2.0*d)
    {
        (void)strcpy(numbuffer,"999e999");
        return (char *)numbuffer;
    }
    if( d < 0.0)
    {
        *cp++ = '-'; 
        d = -d;
    }
    *cp++ = '0' ;
    *cp++ = '.';
    r = 0;
    expo = 0;
    if( d == 0.0  ) 
    {
        *cp++ = '0';
        *cp++ = 0;
        return (char *)numbuffer;
    }
    if (d >= 1.0 )
    {
        while(d >= 1.0e+10)
        {
            r +=10;
            d *= 1.0e-10;
        }
        while(d >= 1.0 )
        {
            r++ ; 
            d *= 1.0e-1;
        }
    }
    else 
    {
        while(d < 1.0e-11)
        {
            r -= 10; 
            d *= 1.0e+10;
        }
        while(d < 0.1)
        {
            r-- ; 
            d *= 10.0 ;
        }
    }
    expo = r ;
#if MAXDIGITS-9<16
    r=MAXDIGITS-9;
#endif
#if MAXDIGITS-9>=16
    r=16;
#endif
    while(r-- >0)
    {
        register int i;
        d *= 1.0e+1; 
        i = (int)d ; 
        d -= (double)i;
        *cp++ =(char)( '0' + (char)i);
    }
    if(*(cp-1)=='9') /* runden */
    {
        while(*--cp=='9');
        if(*cp == '.')
        { 
            *((cp++)-1) = '1';
            *cp++ = '0';
        }
        else
        { 
            (*cp++)++;
            *cp++ = '0';
        }
    }
    while(*--cp=='0');
    if(*cp++ == '.') *cp++ = '0';
    *cp++ = 'e';
    if(expo < 0)  
    { 
        *cp++ = '-' ; 
        expo = -expo;
    }
    else *cp++ = '+';
    if( expo >= 100 )
    {
        *cp++ =(char)('0' + (char)(expo / 100)) ; 
        expo %= 100 ;
    }
    *cp++=(char)('0' + (char)(expo/10));
    *cp++ =(char)('0' + (char)(expo %10));
    *cp = 0;
    return (char *)numbuffer;
}

#endif

GLOBAL string ltoa(v)
    long v;
{
    unsigned long int r;
    register string p;
    int sign;
    sign =  (v < 0);
    r = (sign = (v < 0)) ? -v : v;

    if ( r < 0 )
    {
	ws("warning: -x = x < 0 ");
    }

    p= &numbuffer[MAXDIGITS-2];
    if(r==0l) *p--='0';
    else 
        do 
            *p--=(char)(r%10l)+'0';
        while((r/=10l) != 0);
    if(sign) *p--='-';
    p++;
    numbuffer[MAXDIGITS-1] = 0;
    return p;
}

GLOBAL string itoa(i)
    int i;
{ 
    return ltoa((long)i);
}

GLOBAL void wc(ch)
    register char ch;
{ 
    FORWARD void ws();
    static char c[2] = "x";
    *c = ch;
    ws(c); 
}

GLOBAL void out(f,s)
    register int f;
    register string s;
{
    (void)write(f,s,(unsigned int)strlen(s));
}

GLOBAL void ws(s)
    string s;
{ 
    register int i; 
    register string p;
    for (i=0,p=s; *p ;++i, ++p)
        ;
    if(PROTFLAG && (PRTF > 0))
        (void)write(PRTF,s,(unsigned int)i);
    if(FTYPE(outputfile) == NORMFT)
    {
        if(write(FOUTPTR(outputfile),s,(unsigned int)i) != i && 
           !UserAbort)
            IOError(FNAME(outputfile));
    }
#if WINDOWS
    else if(xWINDOW_ON && FTYPE(outputfile) == WINDOWFT)
        w_puts(FWINPTR(outputfile),s);
#endif
    else SYSTEMERROR("ws.1");
}

GLOBAL int  ERRPOS;

LOCAL boolean unget = false;
LOCAL int FirstCharPos = 0;

LOCAL void fillbuffer()
{
    FirstCharPos = CHARPOS = ERRPOS = LINELENGTH = 0;
#if WINDOWS
    if(FTYPE(inputfile) == NORMFT)
    {
#endif
        if((LINELENGTH=read(FINPTR(inputfile),LINEBUF,BUFLENGTH-1)) < 0)
            IOError((FNAME(inputfile)));
        if (LINELENGTH==0) EOF(inputfile)=true; 
        else EOF(inputfile)=false;
#if WINDOWS
    }
    else if(xWINDOW_ON) /* FTYPE(inputfile) == WINDOWFT */
    {
        if((LINELENGTH=w_gets(FWINPTR(inputfile),LINEBUF,BUFLENGTH-1)) < 0)
            IOError((FNAME(inputfile)));
        if (LINELENGTH==0) EOF(inputfile)=true; 
        else EOF(inputfile)=false;
    }
#endif
    LINEBUF[LINELENGTH] = 0;
}

GLOBAL boolean FILEENDED()
{
    if(!unget && CHARPOS >= LINELENGTH && !ISTTY(inputfile))
        fillbuffer();
    return (!unget && EOF(inputfile) && (CHARPOS >= LINELENGTH));
}

GLOBAL char CH, LASTCH;

GLOBAL void GETCHAR()
{
    if(unget)
    {
        unget = false; 
        return;
    }
    LASTCH= CH;
    if(FILEENDED())
    {
        CH = '\n'; 
        return; 
    }
    if ( CHARPOS >= LINELENGTH )
        fillbuffer();
    if(EOF(inputfile))
    {
	CH = -1 ; 
	return;
    }  
    CH = LINEBUF[CHARPOS++] ;
    if(ECHOFLAG)
        wc(CH);
    if(CH == '\n') 
    {
        FirstCharPos = CHARPOS ; 
        ERRPOS = 0; 
        LINENUMBER++;
    }
    if(PROTFLAG && (PRTF > 0))
        (void)write(PRTF,&CH,1);

}

GLOBAL boolean LINEENDED()
{ 
    if(CHARPOS >= LINELENGTH && !ISTTY(inputfile))
        fillbuffer();
    return (FILEENDED() || LINEBUF[CHARPOS] == '\n');
}


GLOBAL void REGET()
{ 
    unget = true;
}

LOCAL struct { 
    int ERRNR; 
    string ERRMSG; 
    } ERRTAB[]=
{ 
    { HELPE          ,  "helpfile currupted or missed" },
    { ABORTE         ,  "execution aborted" },
    { ARGE           ,  "unsuitable argument(s) to system predicates" },
    { ATOMSPACEE     ,  "out of atom space" },
    { BADARITYE      ,  "arity of functor out of range"  },
    { BADCDDE        ,  "probably malformed ',..'"  },
    { BADCHARE       ,  "character value out of range"  },
    { BADDOTE        ,  "closing bracket missing"  },
    { BADEXPE        ,  "malformed expression"  },
    { BADKETE        ,  "unmatched closing bracket"  },
    { BADTYPE        ,  "bad numerical argument type "  },
    { CALLE          ,  "unsuitable arguments to 'call'"  },
    { COMMENTE       ,  "unterminated comment"  },
    { DEPTHE         ,  "nesting too deep  probably cyclic term"  },
    { DIV0E          ,  "division or mod by zero"  },
    { FPEE	     ,  "floating point error" },
    { EOFE           ,  "unexpected end of file"  },
    { FRAMESPACEE    ,  "out of frame space"  },
    { IOERROR        ,  "I/O error"  },
    { LOCALSPACEE    ,  "out of local stack space"  },
    { NEEDOPE        ,  "infix or postfix operator expected"  },
    { NEEDQUOTEE     ,  "closing quote expected"  },
    { NEEDRANDE      ,  "operand or prefix operator expected"  },
    { NUMBERSYNE     ,  "bad number syntax"  },
    { NVARSE         ,  "out of variable table space"  },
    { PRECE          ,  "operator has unsuitable precedence"  },
    { READNESTE      ,  "nesting too deep in input"  },
    { READSTACKE     ,  "read stack overflow"  },
    { STDFUNCARGE    ,  "function called with wrong argument(s)"  },
    { SYSPROCE       ,  "accessing or modifying system procedures"  },
    { TRAILSPACEE    ,  "out of trail space"  },
    { UNDEFFUNCE     ,  "undefined function in expression"  },
    { VARSPACEE      ,  "out of variable name space"  },
    { WIERDCHE       ,  "illegal character in input"  },
    { aSTRINGSPACEE  ,  "out of string space"  },
    { CANTCR         ,  "can't create file" },
    { CANTOP         ,  "can't open file" },
    { NOTOPEN        ,  "file is not open" },
    { ISTTYE         ,  "file is a tty" },
    { TOMANY         ,  "to many files" },
    { CUROUT         ,  "file is current outputfile" },
    { CURINP         ,  "file is current inputfile" },
    { ONLOUT         ,  "file is only open for output" },
    { ONLINP         ,  "file is only open for input" },
#if SAVE
    { BAD_MAGIC      ,  "bad magic number"},
    { BAD_CHKSUM     ,  "checksum error" },
#endif
    { 0              ,  "unknown error" }
};

GLOBAL ATOM error_string(N)
    ERRORTYPE N;
{
    int I;
    for (I=0; ERRTAB[I].ERRNR != 0; I++)
	if (ERRTAB[I].ERRNR==N) break;
    return LOOKUP(ERRTAB[I].ERRMSG,0,false,true); 
}

GLOBAL void ERROR_MESS(N)
    ERRORTYPE N;
{
    int I;
    for (I=0; ERRTAB[I].ERRNR != 0; I++)
	if (ERRTAB[I].ERRNR==N) break;
    ws("\n");
    if (CALLX != NIL_TERM) 
    {
	TERM OLDBE=BE;
	BE=base(CALLXENV);
	ABORT_WRITE(CALLX);
	BE=OLDBE;
    }
    ws("\nERROR ");
    ws(itoa((int)N));
    ws(": "); 
    ws(ERRTAB[I].ERRMSG); 
    ws(".\n");
}

GLOBAL void ABORT(N)
    ERRORTYPE N;
{
    static int  abort_counter = 0;
    if(N != NOERROR)
    {
        if(abort_counter++  >  2 ||
            (outputfile = OpenFile(phy_name(STDERR_0),write_mode)) < FIRSTFILE)
            outputfile = STDERR;
	ERROR_MESS(N);
#if WINDOWS
        if(xWINDOW_ON && FTYPE(outputfile) == WINDOWFT)
	    show_window(FWINPTR(outputfile));
#endif
    }
    ERRORFLAG = NOERROR;
    abort_counter = 0;
    ABORT_JMP();
}

LOCAL file old_out_file;
LOCAL boolean in_warning = false;

GLOBAL void START_WARNING()
{
    in_warning = true;
    old_out_file = outputfile;
    if((outputfile = OpenFile(phy_name(STDWRN_0),write_mode)) 
		 < FIRSTFILE)
	outputfile = STDOUT;
}

GLOBAL void END_WARNING()
{
    if(! in_warning)
	SYSTEMERROR("END_WARNING without START_WARNING");
    outputfile = old_out_file;
}

GLOBAL void ARGERROR()
{ 
    ERROR(ARGE); 
}

GLOBAL void ERROR(N)
    ERRORTYPE N;
{
    ERRORFLAG = N;
    ERROR_JMP();
}

GLOBAL void FileError(error)
    ERRORTYPE error;
{
    if(!FERRORFLAG) return;
    ERROR(error);
}


GLOBAL void SYNERROR(N)
    ERRORTYPE N;
{ 
    int  I;
    if((outputfile = OpenFile(phy_name(STDERR_0),write_mode)) < FIRSTFILE)
        outputfile = STDERR;
    if(FLOGNAME(inputfile) != STDIN_0)
    {
        ws("\nSyntaxerror at line: ");
        ws(itoa(LINENUMBER + 1));
        ws("  position: ");
        ws(itoa(CHARPOS - FirstCharPos));
        ws("\n");
    }
    if (!ECHOFLAG) 
    {
        for(I=FirstCharPos;LINEBUF[I]!='\n' && I < LINELENGTH; I++)
            wc(LINEBUF[I]);
    }
    ws("\n");
    for (I=FirstCharPos; I<ERRPOS-1; ++I)
        if (LINEBUF[I]=='\t') ws("\t"); 
        else ws(" ");
    ws("^");
    if (inputfile == STDIN || FTYPE(inputfile)==WINDOWFT)
        CHARPOS= LINELENGTH;
    CALLX=NIL_TERM;
    ABORT(N);
}

GLOBAL void SYSTEMERROR(m)
    string m;
{ 
    out(2,"\n\n[System Error in: "); 
    out(2,m); 
    out(2,"]\n"); 
    EXIT_JMP(1);
}

LOCAL file f;
LOCAL TERM filename;
LOCAL ATOM matom;

GLOBAL boolean DOSEE()
{
    CHECKATOM(A0);
    matom = name(A0);
    if(matom == USER_0)
        matom = STDIN_0;
    f=OpenFile(phy_name(matom),read_mode);
    if(f == NOFILE)
    {
	FileError(CANTOP); return false;
    }
    else if(f == ERRFILE)
    {
	FileError(TOMANY); return false;
    }
    else
    {
        if(f==outputfile && FTYPE(f) == NORMFT)
        { 
            FileError(CUROUT); 
            return false; 
        }
        if(!CANREAD(f))
        { 
            FileError(ONLOUT); 
            return false; 
        }
        inputfile = f;
        if(!ISINPUT(inputfile))
        {
            ISINPUT(inputfile)=true;
            CHARPOS=LINELENGTH = 0;
        }
    }
    FLOGNAME(inputfile) = copyatom(matom);
    return true;
}

GLOBAL boolean DOOPEN()
{
    CHECKATOM(A0);
    matom = name(A0);
    if(matom == USER_0) return true; /* user is open ! */
    filename = phy_name(matom);
    f=OpenFile(filename,read_write);
    if(f == NOFILE)
    {
        FileError(CANTOP);
        return false;
    }
    else if(f == ERRFILE)
    {
        FileError(TOMANY);
        return false;
    }
    FLOGNAME(f) = copyatom(matom);
    return true;
}

GLOBAL boolean DOCLOSE()
{
    CHECKATOM(A0);
    matom = name(A0);
    if(matom == USER_0) return true; /* ! */
    filename = phy_name(matom);
    if((f=OpenFile(filename,look_mode)) == NOFILE)
    {
        FileError(NOTOPEN);
        return false;
    }
    CloseFile(f);
    if(inputfile == f)
        if((inputfile=OpenFile(phy_name(STDIN_0),read_mode)) < FIRSTFILE)
            inputfile = STDIN;
    if(outputfile == f)
        if((outputfile=OpenFile(phy_name(STDOUT_0),write_mode)) < FIRSTFILE)
            outputfile = STDOUT;
    return true;
}


GLOBAL boolean DOTELL()
{
    CHECKATOM(A0);
    matom = name(A0);
    if(matom == USER_0)matom = STDOUT_0;
    filename = phy_name(matom);
    f=OpenFile(filename,write_mode);
    if(f == NOFILE)
    {
        FileError(CANTCR); 
        return false;
    }
    else if(f == ERRFILE)
    {
        FileError(TOMANY); 
        return false;
    }
    else
    {
        if(f==inputfile && FTYPE(f) == NORMFT)
        { 
            FileError(CURINP); 
            return false; 
        }
        if(!CANWRITE(f))
        { 
            FileError(ONLINP); 
            return false; 
        }
        outputfile = f;
        if(ISINPUT(outputfile) && FTYPE(f) == NORMFT)
        {
            ISINPUT(outputfile)=false;
            (void)lseek(FOUTPTR(f),
                (long)(FCHARPOS(outputfile)-
                FBUFLENGTH(outputfile)),1);
        }
    }
    FLOGNAME(outputfile) = copyatom(matom);
    return true;
}


GLOBAL boolean DOSEEK()
{
    long l;
    boolean res = true; /* initialisation for lint only */

    CHECKATOM(A0);
    if(name(A0)==USER_0) ARGERROR();
    matom = name(A0);
    filename = phy_name(matom);
    if((f=OpenFile(filename,look_mode))<FIRSTFILE)
    { 
        FileError(NOTOPEN); 
        return false; 
    }
    if((f<=STDTRACE)||ISTTY(f)||FTYPE(f) != NORMFT)
    { 
        FileError(ISTTYE); 
        return false; 
    }
    switch(name(A1))
    {
    case END_0:
        res = lseek(FINPTR(f),0L,2) >= 0L;
        FBUFLENGTH(f)=FCHARPOS(f)=FLINENO(f)=0;
	FUNGET(f) = false;
        break;
    case INTT: 
        l=(long)ival(A1); 
        goto contseek;
#if LONGARITH
    case LONGT: 
        l =  longval(A1);
#endif
contseek:  
        res = lseek(FINPTR(f),(l < 0L ? -l : l),(l >= 0L) ? 0 : 2) 
              >= 0L;
        FBUFLENGTH(f)=FCHARPOS(f)=FLINENO(f)=0;
	FUNGET(f) = false;
        break;
    case UNBOUNDT:
        l=lseek(FINPTR(f),(long)0,1);
        l = l-(long)
            (ISINPUT(f) ? (FBUFLENGTH(f)-FCHARPOS(f)):0);
#if LONGARITH
        return LONGRES(A1,l);
#endif
#if ! LONGARITH
        return INTRES(A1,(int)l);
#endif
    default: 
        ARGERROR();
    }
    return res;
}

LOCAL TERM oldfilename;
GLOBAL void getinfile()
{
    if((inputfile=OpenFile(oldfilename,read_mode)) < FIRSTFILE)
        SYSTEMERROR("getinfile.1");
}

GLOBAL void setinfile()
{
    oldfilename = FNAME(inputfile);
    if((inputfile = OpenFile(phy_name(STDIN_0),read_mode)) < FIRSTFILE)
        getinfile();
}

GLOBAL void getoutfile()
{
    if((outputfile=OpenFile(oldfilename,write_mode)) < FIRSTFILE)
        SYSTEMERROR("getoutfile.1");
}


GLOBAL void setoutfile()
{
    oldfilename = FNAME(outputfile);
    if((outputfile = OpenFile(phy_name(STDOUT_0),write_mode)) < FIRSTFILE)
        getoutfile();
}

GLOBAL boolean DOGET0()
{
        return INTRES(A0,FILEENDED() ? -1 : (GETCHAR(),(int)CH));
}

GLOBAL boolean DOGET()
{
    register int ch;
nextch:
    if(FILEENDED()) ch = -1;
    else 
    { 
        GETCHAR();
        if (CH<=' ' || CH > '~') goto nextch;
        ch = (int)CH;
    }
    return INTRES(A0,ch);
}

GLOBAL void DOSKIP()
{
    register int n;
    n=(INTVALUE(A0) & 0xff);
nextch:
    if (FILEENDED()) return;
    GETCHAR();
    if (CH!=(char)n) goto nextch;
}

GLOBAL boolean DOASK()
{
    register int ch;
nextch:
    if (FILEENDED()) return true;
    GETCHAR();
    if (CH==0) goto nextch;
    ch = CH;
    while((CH!=10) && !FILEENDED())
        GETCHAR();
    return INTRES(A0,ch);
}

GLOBAL void DOTAB()
{  
    register int n;
    n=INTVALUE(A0);
    while (n-->0) ws(" ");
}

GLOBAL void DOPUT()
{
    if(name(A0) == CONS_2 || name(A0) == STRING_CONS)
    {
	while(name(A0) == CONS_2 || name(A0) == STRING_CONS)
	{
	    wc((char)(INTVALUE(son(A0)) & 255)); 
	    A0=arg2(A0); 
	}
    }
    else if(name(A0) == NIL_0 || name(A0) == STRING_NIL)
	/*empty*/ ;
    else
        wc((char)((INTVALUE(A0))&255));
}


GLOBAL boolean DOFASSIGN()
{
    register CLAUSE CL,CC;
    TERM F;

    if(!ground(A0,MAXDEPTH)) ARGERROR();
    if(!ground(A1,MAXDEPTH))
    {
        CL = clause(FNAME_2);
        while(CL != NIL_CLAUSE)
        {
            if(nvars(CL) != 0) SYSTEMERROR("assign");
            F = br(son(head(CL)));
            if(!UNI(A0,F)) { 
                CL = nextcl(CL); 
                continue; 
            }
            /* found */
            return UNI(A1,son(head(CL)));
        }
        return UNI(A0,A1);
    }

    CL = CC = clause(FNAME_2);
    while(CL != NIL_CLAUSE)
    {
        if(nvars(CL) != 0) SYSTEMERROR("assign");
        F = br(son(head(CL)));
        if(!UNI(A0,F))
        { 
            CC = CL; 
            CL = nextcl(CL); 
            continue; 
        }
        if(CL == clause(FNAME_2)) clause(FNAME_2) = nextcl(CL);
        else nextcl(CC) = nextcl(CL);
        FreeClause(CL);
        break;
    }
    if(UNI(A0,A1)) return true;
    F = stackvar(2);
    (void)UNI(F,A1);
    (void)UNI(br(F),A0);
    CL = NewClause(mkfunc(FNAME_2,F),NIL_TERM , false);
    nextcl(CL) = clause(FNAME_2); 
    clause(FNAME_2) = CL;
    return true;
}

GLOBAL void DOCLS()
{
#if WINDOWS
    if(xWINDOW_ON && FTYPE(outputfile) == WINDOWFT)
        w_cls(FWINPTR(outputfile));
    else 
#endif
        ws(t_cls());
}

GLOBAL boolean DOGOTOXY()
{ 
    register int S,Z;

#if WINDOWS
    if(xWINDOW_ON && FTYPE(outputfile) == WINDOWFT) 
    {
	if(name(A1) == UNBOUNDT && name(A0) == UNBOUNDT)
	{
	    return INTRES(A0,w_get_x_pos(FWINPTR(outputfile))) &&
		   INTRES(A1,w_get_y_pos(FWINPTR(outputfile)));
	}
	if(name(A0) == UNBOUNDT &&  
	   ! INTRES(A0,w_get_x_pos(FWINPTR(outputfile))))
	   return false;
	if(name(A1) == UNBOUNDT &&  
	   ! INTRES(A1,w_get_y_pos(FWINPTR(outputfile))))
	   return false;
    }
#endif

    S = INTVALUE(A0);
    Z = INTVALUE(A1);
#if WINDOWS
    if(xWINDOW_ON && FTYPE(outputfile) == WINDOWFT)
        w_gotoxy(FWINPTR(outputfile),S,Z);
    else 
#endif
        ws(t_gotoxy(S,Z));

    return true;
}


/****************** I N I T I A L I S A T I O N *****************/


LOCAL TERM node(A)
    ATOM A;
{ 
    TERM T;
    T=heapterm(1);
    name(T)=A; 
    son(T)=NIL_TERM; 
    return T;
}

GLOBAL void Init_Io()
{
    file i;
    static int first_time = 1;
    if(!first_time) 
    { 
        EOF(inputfile) = false; 
        return; 
    }
#if DEBUG
    if(DEBUGFLAG)
        out(2,"Init_Io\n");
#endif
    first_time = 0;
    for(i = FIRSTFILE; i <= LASTFILE; ++i)
        FNAME(i) = NIL_TERM;

    FNAME(STDIN)=node(STDIN_0); 
    EOF(STDIN)=false; 
    ISTTY(STDIN)=isatty(0);
    CANREAD(STDIN)=!(CANWRITE(STDIN)=false);
    ISINPUT(STDIN)=true;
    FCHARPOS(STDIN)=FBUFLENGTH(STDIN)=0;
    FUNGET(STDIN) = false;
    FLINENO(STDIN)=1;
    FLOGNAME(STDIN) = STDIN_0;
        FTYPE(STDIN) = NORMFT;
        FOUTPTR(STDIN)=FINPTR(STDIN) = 0;
#if WINDOWS
        if(xWINDOW_ON)
        {
            FTYPE(STDIN) = WINDOWFT;
            FWINPTR(STDIN) = STDWIN;
            CANREAD(STDIN) = CANWRITE(STDIN) = true;
        }
#endif

    FNAME(STDOUT)=node(STDOUT_0); 
    EOF(STDOUT)=false; 
    ISTTY(STDOUT)=isatty(1);
    CANREAD(STDOUT)=!(CANWRITE(STDOUT)=true);
    ISINPUT(STDOUT)=false;
    FLOGNAME(STDOUT) = STDOUT_0;
        FTYPE(STDOUT) = NORMFT;
        FOUTPTR(STDOUT)=FINPTR(STDOUT) = 1;
#if WINDOWS
        if(xWINDOW_ON)
        {
            FTYPE(STDOUT) = WINDOWFT;
            FWINPTR(STDOUT) = STDWIN;
            CANREAD(STDOUT) = CANWRITE(STDOUT) = true;
        }
#endif

    FNAME(STDERR)=node(STDERR_0); 
    EOF(STDERR)=false; 
    ISTTY(STDERR)=isatty(2);
    CANREAD(STDERR)=!(CANWRITE(STDERR)=true);
    ISINPUT(STDERR)=false;
    FLOGNAME(STDERR) = STDERR_0;
    FTYPE(STDERR) = NORMFT;
    FOUTPTR(STDERR)=FINPTR(STDERR) = 2;
#if WINDOWS
    if(xWINDOW_ON)
    {
	FTYPE(STDERR) = WINDOWFT;
	FWINPTR(STDERR) = STDWIN;
	CANREAD(STDERR) = CANWRITE(STDERR) = true;
    }
#endif

    /*
    STDWRN auf STDOUT gelegt (siehe datab.c / Init_Datab())
    */

    FNAME(STDTRACE)=node(STDTRACE_0); 
    EOF(STDTRACE)=false; 
    ISTTY(STDTRACE)=isatty(0);
    CANREAD(STDTRACE)=CANWRITE(STDTRACE)=true;
    ISINPUT(STDTRACE)=false;
    FLOGNAME(STDTRACE) = STDTRACE_0;
    FTYPE(STDTRACE) = NORMFT;
    FOUTPTR(STDTRACE)=1;
    FINPTR(STDTRACE) = 0;
#if WINDOWS
        if(xWINDOW_ON)
        {
            FTYPE(STDTRACE) = WINDOWFT;
            FWINPTR(STDTRACE) = STDWIN;
            CANREAD(STDTRACE) = CANWRITE(STDTRACE) = true;
        }
#endif

#if HELP
    FNAME(STDHELP)=node(STDHELP_0); 
    EOF(STDHELP)=false; 
    ISTTY(STDHELP)=isatty(0);
    CANREAD(STDHELP)=CANWRITE(STDHELP)=true;
    ISINPUT(STDHELP)=false;
    FLOGNAME(STDHELP) = STDHELP_0;
    FTYPE(STDHELP) = NORMFT;
    FOUTPTR(STDHELP)=1;
    FINPTR(STDHELP) = 0;
#if WINDOWS
        if(xWINDOW_ON)
        {
            FTYPE(STDHELP) = WINDOWFT;
            FWINPTR(STDHELP) = STDWIN;
            CANREAD(STDHELP) = CANWRITE(STDHELP) = true;
        }
#endif
#endif

    inputfile=STDIN;
    outputfile=STDOUT;
    if(tempterm == NIL_TERM)
        tempterm = node(NIL_ATOM); 
    if(PROTFILE && *PROTFILE && (PRTF = creat(PROTFILE,0666))== -1)
    {
        out(2,"\nfatal: can't open ");
        out(2,PROTFILE);
        out(2,"\n");
        EXIT_JMP(1);
    }
}

boolean DOWINDOW()
{
#if NEW_WINDOWS
    if(!xWINDOW_ON)
    {
	Init_Windows();

	FTYPE(STDIN) = WINDOWFT;
	FWINPTR(STDIN) = STDWIN;
	CANREAD(STDIN) = CANWRITE(STDIN) = true;

	FTYPE(STDOUT) = WINDOWFT;
	FWINPTR(STDOUT) = STDWIN;
	CANREAD(STDOUT) = CANWRITE(STDOUT) = true;

	FTYPE(STDERR) = WINDOWFT;
	FWINPTR(STDERR) = STDWIN;
	CANREAD(STDERR) = CANWRITE(STDERR) = true;

	FTYPE(STDTRACE) = WINDOWFT;
	FWINPTR(STDTRACE) = STDWIN;
	CANREAD(STDTRACE) = CANWRITE(STDTRACE) = true;

#if HELP
	FTYPE(STDHELP) = WINDOWFT;
	FWINPTR(STDHELP) = STDWIN;
	CANREAD(STDHELP) = CANWRITE(STDHELP) = true;
#endif
	xWINDOW_ON=true;
    }
#endif
    return xWINDOW_ON;
}
/* end of file */
