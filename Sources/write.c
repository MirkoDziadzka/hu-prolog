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
#include "types.h"
#include "atoms.h"
#include "files.h"
#include "extern.h"

#include "charcl.h"

LOCAL boolean QUOTE,LISTFLAG,DISPLFLAG;
LOCAL boolean last_was_numb;

LOCAL void write_quoted_char(ch)
    char ch;
{
    if(ch == '\t') 	  ws("\\t");
    else if(ch == '\n')   ws("\\n");
    else if(ch == '\007') ws("\\a");
    else if(ch == '\r')   ws("\\r");
    else if(ch == '\b')   ws("\\b");
    else if(ch == '\f')   ws("\\f");
    else if(ch == '\013') ws("\\v");
    else if(ch == '\\')   ws("\\\\");
    else if(ch == '\'')   ws("\\\'");
    else if(ch == '\"')   ws("\\\"");
    else if(charclass(ch) == 0)
    { 
	wc('\\');
	wc((((char)(ch & 0300)) >> 6) + '0');
	wc((((char)(ch & 0070)) >> 3) + '0');
	wc(((char)(ch & 0007)) + '0');
    }
    else wc(ch);
}

GLOBAL void wq(A)
ATOM A;
{/* quoted output if necessary */
    int nIDENT=0,aIDENT=0;
    int notfirst = 0;
    string s,ss;
    ss= s = tempstring(atomstring(A));
    if(QUOTE==false) goto noquote;
    if(s[0] == '.' && s[1] == '\0')
	goto quote;
    do {
        switch(charclass(*s))
        {
        case BC:
        case DC: 
            if(!notfirst)goto quote;
        case SC: 
            ++nIDENT; 
            break;
        case OC: 
            ++aIDENT;
            break;
        default: 
            goto quote;
        }
        s++;
        notfirst++;
    }while(*s);
    if(nIDENT &&  aIDENT) goto quote;
noquote:
    ws(ss);
    return;
quote: 
    ws("\'");
    while(*ss)
	write_quoted_char(*ss++);
    ws("\'");
    return;
}

LOCAL void WRITETERM (Y, P, DEPTH)
    TERM Y;
    int P;
    int DEPTH;
{
    ATOM A;
    last_was_numb=false;
    if(UserAbort ) 
        return;
    if (DEPTH <= 0)  
    { 
        ws("___"); 
        return; 
    }
    if(!LISTFLAG || name(Y) == VART) 
    {
        deref(Y);
    }
    switch (A=name(Y))
    {
    case GOTOT:
        ws(" $goto(");
        Y = son(Y);
        if(Y != NIL_TERM && name(Y) != NIL_ATOM)
        {
            WRITETERM(Y,0,DEPTH-1);
            if(name(Y) == GOTOT) break;
            next_br(Y);
            while(Y != NIL_TERM && name(Y) != NIL_ATOM )
            {
                ws(", ");
                WRITETERM(Y,0,DEPTH-1);
                if(name(Y) == GOTOT) break;
                if(UserAbort) break;
                next_br(Y);
            }
        }
        ws(")");
        break;
    case INTT: 
        ws(itoa(ival(Y))); 
        last_was_numb=true; 
        break;
#if LONGARITH
    case LONGT: 
        ws(ltoa(longval(Y))); 
        last_was_numb=true; 
        break;
#endif
#if REALARITH
    case REALT: 
        ws(ftoa(realval(Y))); 
        last_was_numb=true; 
        break;
#endif
    case UNBOUNDT: 
        ws("_"); 
        ws(itoa((int)((MAX_TERM-Y)/TERM_UNIT)));
        break;

    case SKELT:
        ws("_"); 
        ws(itoa(offset(Y))); 
        break;

    case CURLY_1:
        ws("{"); 
        WRITETERM(son(Y),MAXPREC,DEPTH-1);
        if(UserAbort ) return;
        ws("}"); 
        break;

    case STRING_CONS:
	{
	    TERM Z = Y;
	    if(!islist(Y, true, LISTFLAG))
		goto no_string;
	    wc('\"');
	    while(name(Z) == STRING_CONS)
	    {
		write_quoted_char((char)
		       ival(LISTFLAG ? son(Z) : arg1(Z)));
		Z = LISTFLAG ? br(son(Z)) : arg2(Z);
	    }
	    wc('\"');
	}
	break;
#if !STRINGS_AS_INT_LISTS
    case CONS_2:
#endif
no_string:
        if(!DISPLFLAG)
        { 
            int  N;
            TERM  Z;
            Z=Y; 
            N=0;
            ws("[");
            WRITETERM(son(Y),SUBPREC,DEPTH-1);
            if(UserAbort ) return;
            N= 1;
            if (LISTFLAG) Z=br(son(Y)); 
            else Z=arg2(Y);
            while (N++!=WRITELENGTH && name(Z)==CONS_2 )
            { 
                ws(", ");
                WRITETERM(son(Z),SUBPREC,DEPTH-1);
                if(UserAbort ) return;
                if (LISTFLAG) Z=br(son(Z)); 
                else Z=arg2(Z);
            }
            if (name(Z)!=NIL_0)
            { 
                ws(" | "); 
                if(N<WRITELENGTH) 
                    WRITETERM(Z,SUBPREC,DEPTH-1); 
                else ws(" ___");
            }
            ws("]");
            break;
        }


    default:
/*
	    if(is_heap_term(Y)) ws("<H>");
	    else if(is_stack_term(Y)) ws("<S>");
	    else ws("<<"),ws(itoa(Y-MIN_TERM)),ws(">>");
*/

#if HIGHER_ORDER
        if(var_call(A) && arity(A) > 0)
	{
            int i,aar;
            TERM  S;

            aar = arity(A);
            S = son(Y);

	    WRITETERM(S,SUBPREC,DEPTH-1);
	    next_br(S);

            ws("(");

            for(i=2;i<=aar;i++)
            {  
                WRITETERM(S,SUBPREC,DEPTH-1);
                if(UserAbort ) return;
                next_br(S);
                if(i < aar) ws(", ");
            }
            ws(")");
            break;
	}
#endif
        if(arity(A)==0)
        {     
/*
	    if(is_heap_atom(A)) ws("<h>");
	    else if(is_stack_atom(A)) ws("<s>");
	    else ws("<<"),ws(itoa(A-MIN_ATOM)),ws(">>");
*/
            if(A != NIL_0 && A != CURLY_0 && A != CUT_0) wq(A);
            else ws(tempstring(atomstring(A)));
            break;
        }


        if (oclass(A)==NONO || arity(A)>2 || DISPLFLAG)
        /*------------------------------------*/
        { 
            int i,aar;
            TERM  S;
/*
	    if(is_heap_atom(A)) ws("<h>");
	    else if(is_stack_atom(A)) ws("<s>");
	    else ws("<<"),ws(itoa(A-MIN_ATOM)),ws(">>");
*/

            wq(A);
            ws("(");
            aar = arity(A);
            S = son(Y);
            for(i=1;i<=aar;i++)
            {  
                WRITETERM(S,SUBPREC,DEPTH-1);
                if(UserAbort ) return;
                next_br(S);
                if(i < aar) ws(", ");
            }
            ws(")");
            break;
        }

    /* operator notation */

        if (P<oprec(A)) ws("(");
        switch (oclass(A))
        { 
        case FXO: 
        case FYO:
            wq(A); 
            ws(" "); 
            WRITETERM(son(Y),RPREC(A),DEPTH-1); 
            break;
        case XFO: 
        case YFO:
            WRITETERM(son(Y),LPREC(A),DEPTH-1); 
            if(UserAbort ) return;
            ws(" "); 
            wq(A); 
            break;
        case XFXO: 
        case XFYO: 
        case YFXO:
            WRITETERM(son(Y),LPREC(A),DEPTH-1);
            if(UserAbort ) return;
            if (A==CONS_2 && last_was_numb) ws(" ");
            if (A!=COMMA_2 && A!=SEMI_2 && A!=CONS_2) {
                ws(" "); 
                wq(A);
            }
            else ws(tempstring(atomstring(A)));
            if (A!=CONS_2 && A!=NL_2) ws(" ");
            WRITETERM(br(son(Y)),RPREC(A),DEPTH-1);
            if(UserAbort ) return;
            break;
        default: 
            SYSTEMERROR("WRITETERM.1");
        }
        if (P<oprec(A)) ws(")");
        break;

    }
}

GLOBAL void DISPLAY(T)
TERM T;
{ 
    QUOTE = true; 
    LISTFLAG = false; 
    DISPLFLAG = true;
    WRITETERM(T,MAXPREC,WRITEDEPTH);
    QUOTE = false;
}

GLOBAL void WRITEOUT (X,quote)
TERM X;
boolean quote;
{ 
    QUOTE = quote; 
    DISPLFLAG = false; 
    LISTFLAG= false;
    WRITETERM(X,MAXPREC,WRITEDEPTH);
    QUOTE=false;
}

GLOBAL void ABORT_WRITE(T)
register TERM T;
{
    QUOTE = false; 
    DISPLFLAG = false; 
    LISTFLAG= false;
    WRITETERM(T,MAXPREC,10);
}


GLOBAL void LISTOUT (X)
TERM X;
{ 
    LISTFLAG= true; 
    DISPLFLAG = false;
    QUOTE= true; 
    WRITETERM(X,SUBPREC,WRITEDEPTH);
    QUOTE = false;
}


/* Output a trace message. */

#if !NO_TRACE
GLOBAL ENV TRACE_GOON=0;

GLOBAL boolean TRACE (MESS, Y, ENVP)
    ATOM MESS;
    TERM Y;
    ENV ENVP;
{ 
    boolean spyflag= false, answer= true;
    boolean mustread= true;
    boolean again = false;
    TERM oldinfile,oldoutfile;

    E=ENVP; 
    BE=base(ENVP);

    if (TRACE_GOON && E>TRACE_GOON) return true;
    if (TRACE_GOON && E==TRACE_GOON && MESS==REDO_0) return true;
    deref_(Y,BE); /* @@@@ */
#if DEBUG
    if(!DEBUGFLAG)
#endif
    {
	if(name(Y) == GOTOT)
	    return true;
        if (name(Y)==COMMA_2 || name(Y)==SEMI_2
#ifdef muell
	|| 
            (!func_atom(name(Y)) && name(Y)!=INTT && name(Y) != VART)
#endif
	)
            return true;
        if(stringchar(atomstring(name(Y)),0) == '$')
            return true;
    }
    TRACE_GOON=0;
    spyflag = spy(name(Y));
    if(!spyflag && !TRACING)
	return true;

    oldinfile=FNAME(inputfile);
    if((inputfile=OpenFile(phy_name(STDTRACE_0),read_mode)) < FIRSTFILE)
	if((inputfile=OpenFile(phy_name(STDIN_0),read_mode)) < FIRSTFILE)
	    inputfile = STDIN;
    oldoutfile=FNAME(outputfile);
    if((outputfile=OpenFile(phy_name(STDTRACE_0),write_mode)) < FIRSTFILE)
	if((outputfile=OpenFile(phy_name(STDOUT_0),write_mode)) < FIRSTFILE)
	    outputfile = STDOUT;

    ws(spyflag ? "*" : " ");
    ws("("); 
    ws(itoa(E-MIN_ENV)); 
    ws(")");
    switch(MESS)
    { 
    case CALL_0:    ws("\tCALL: "); break;
    case REDO_0:    ws("\tREDO: "); break;
    case PROVED_0:  ws("\tEXIT: "); break;
    case FAILED_0:  ws("\tFAIL: "); break;
    default:        ws("\tERROR: "); break;
    }
    WRITEOUT(Y,true);
    if (MESS==CALL_0 || MESS==REDO_0)
    {
mesg:
        ws("   [sanft?\\n] ");
nextch:
        if (FILEENDED()) goto ret;
        GETCHAR();
        if(!mustread && (CH != '\n'))goto nextch;
        switch(CH)
        {
        case 's': 
        case 'S':   TRACE_GOON=E; mustread = false; goto nextch;
        case 'a': 
        case 'A':   TRACE_GOON=0; ABORT(ABORTE);
        case 'f': 
        case 'F':  answer=false; mustread = false; 
                   TRACE_GOON=0; goto nextch;
        case 'n': 
        case 'N':  TRACING=false; SPYTRACE= SPYCOUNT;
                   TRACE_GOON = 0; mustread = false; goto nextch;
        case 't': 
        case 'T':  TRACING=true; SPYTRACE= true;
                   mustread = false; goto nextch;
        case '?':
            ws(" [s]kip\n");
            ws(" [a]bort\n");
            ws(" [n]otrace\n");
            ws(" [f]ail\n");
            ws(" [t]race\n");
            ws(" [\\n]  next");
            mustread=false;
            again = true;
            goto mesg;
        case '\n':           
            if(!again) goto ret;
            again = false;
            mustread = true;/* no break */
        default:             
            goto nextch;
        }
    }
    else ws("\n");
ret:
    inputfile=OpenFile(oldinfile,read_mode);
    outputfile=OpenFile(oldoutfile,write_mode);
    return answer;
}

#endif



/* end of file */
