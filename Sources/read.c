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

#define debug(X)	out(2,"[");out(2,__FILE__);out(2,":");out(2,itoa(__LINE__));out(2,"  ");out(2,X);out(2,"]");
#define debug(X)	/**/

#include "config.h"
#include "types.h"
#include "atoms.h"
#include "files.h"
#include "extern.h"

#include "charcl.h"


/* The precedence for a left operand of a. */
GLOBAL int LPREC (A)
    ATOM A;
{ 
    switch (oclass(A))
    { 
    case XFO:
    case XFXO:
    case XFYO:
        return oprec(A)-1;
    default:
        return oprec(A);
    }
}

/* The precedence for a right operand of a. */
GLOBAL int RPREC (A)
    ATOM A;
{ 
    switch (oclass(A))
    { 
    case FXO:
    case XFXO:
    case YFXO:
        return oprec(A)-1;
    default:
        return oprec(A);
    }
}

/*
   Input and parse a Prolog sentence and build a term from it.  The
   finite state part of the parser is characterized by the variables
   'context' and 'expected'.
   'context' indicates the construct being parsed:
      outerK       The outermost level of a sentence.
      innerK       An expression in parentheses.
      funcK        The arguments of a functor.
      listK        The elements of a list.
      endlistK     A list continuation (between '|' or ',..'
                   and ']' in a list).
      curlyK       An expression in curly brackets.
   'expected' indicates whether the next symbol is to be an operator
   (opX) or an operand (randX).
   Two stacks are used: one, represented by the array 'stack', to hold
   parts of incompletely parsed terms, the other, represented by the
   array 'statestack', to hold contextual information during parsing of
   nested constructs. In fact, the parsing algorithm corresponds to a
   stack machine with a single stack, but two stacks are used only as a
   matter of convenience.
*/

#define OUTERK 0
#define INNERK 1
#define FUNCK 2
#define LISTK 3
#define ENDLISTK 4
#define CURLYK 5
#define SpezialCK	6
#define SpezialPK	7
#define STATE  int

#define TERML 0
#define OPL 1
#define FUNCL 2
#define MARKL 3
#define MATOML	4
#define ELEMTAG int

LOCAL STATE  CONTEXT;
LOCAL boolean OPEXPECTED;
LOCAL int  HIPREC, LOPREC;
LOCAL int  RTOP;

LOCAL TERM TSTACK[READSIZE];
LOCAL ATOM ASTACK[READSIZE];
LOCAL ELEMTAG TAGSTACK[READSIZE];

LOCAL void RPOP(PA,PT)
register ATOM *PA; 
register TERM *PT;
{ 
    *PA=ASTACK[RTOP]; 
    *PT=TSTACK[RTOP]; 
    RTOP--; 
}

LOCAL void RPUSH (T,A,X)
register ELEMTAG T; 
register ATOM A; 
register TERM X;
{ 
    if (RTOP>=READSIZE) SYNERROR(READSTACKE);
    RTOP++;
    TAGSTACK[RTOP]= T; 
    ASTACK[RTOP]=A; 
    TSTACK[RTOP]=X;
}

LOCAL  int STOP;
LOCAL  int READCONTEXT[READDEPTH];
LOCAL  int READPREC[READDEPTH];

LOCAL void SAVECONTEXT()
{ 
    if (STOP>=READDEPTH) SYNERROR(READNESTE);
    READCONTEXT[STOP]= CONTEXT; 
    READPREC[STOP]= HIPREC; 
    STOP++;
}

LOCAL void RESTORECONTEXT()
{ 
    STOP--; 
    CONTEXT= READCONTEXT[STOP]; 
    HIPREC= READPREC[STOP];
}

LOCAL int  VARCT;

GLOBAL int VARCOUNT()
{
    return VARCT;
}

LOCAL string aIDENT[MAXVARS];
LOCAL TERM VAR_TAB[MAXVARS];
LOCAL int counter[MAXVARS];

GLOBAL TERM VARTERM()
{ 
    TERM Q,R; 
    int I;
    if (VARCT==0) return NIL_TERM;
    R=NIL_TERM;
    for (I=VARCT-1; 0<=I; I--)
    { 
        Q=mk2sons(LOOKUP(aIDENT[I],0,false,true),NIL_TERM,
            VART,VAR_TAB[I]);
        if (R != NIL_TERM)  
            R=mkfunc(NL_2,mk2sons(ISEQ_2,Q,VART,R));
        else R=mkfunc(ISEQ_2,Q);
    }
    if (R != NIL_TERM)
    { 
        Q=mk2sons(WRITE_1,R,GOTOT,
            mk2sons(NOT_1,mkfunc(ASK_1,mkint(59)),NIL_ATOM,NIL_TERM));
        return Q;
    }
    return NIL_TERM;
}


/*
      Collapse items on the stack. Before each reduction step, the
      operator a on top of the stack is "balanced" against the
      precedences p = b@.oprec and lp = Lprec(b) of a new operator b,
      to see if a could be a left operand of b, or b a right operand of
      a. If neither is possible or both are possible, a precedence
      conflict is reported.  If only the first is possible, a reduction
      step is taken. If only the second is possible, reduction is
      complete.
*/

LOCAL void REDUCE (P, LP)
int P, LP;
{ 
    TERM  X, Y;
    ATOM  A,XA,YA;
    RPOP(&XA,&X);
    while (TAGSTACK[RTOP]==OPL)
    { 
        A= ASTACK[RTOP];
        if (RPREC(A)>=P)
            if (oprec(A)<=LP) SYNERROR(PRECE);
            else break;
        else
            if (oprec(A)>LP) SYNERROR(PRECE);
            else 
            { 
                RTOP--;
                switch (oclass(A))
                { 
                case FXO:
                case FYO: X=mkfunc(XA,X); XA=A; break;
                case XFXO:
                case XFYO:
                case YFXO: RPOP(&YA,&Y); X=mk2sons(YA,Y,XA,X); XA=A; 
                case XFO:
                case YFO:
                case NONO: break;
                default: SYSTEMERROR("REDUCE");
                }
            }
    }
    RPUSH(TERML,XA,X);
}

/*
      Attempt to force the state required for a delimiter.
      This state must satisfy the predicate
         (expected = opX) and (context in s).
      If initially (expected = randX) and the top item on the stack
      is a prefix operator, this operator is converted to an atom.
      This allows for constructions such as (?-) in which a prefix
      operator occurs as an atom.
*/

LOCAL void CHECKDELIM ()
{ 
    ATOM  A; 
    TERM X;
    if (!OPEXPECTED)
    { 
        if (TAGSTACK[RTOP]!=OPL) SYNERROR(NEEDRANDE);
        RPOP(&A,&X);
        if (oclass(A)!=FXO && oclass(A)!=FYO) SYNERROR(NEEDRANDE);
        RPUSH(TERML,LOOKATOM(A,0,true),NIL_TERM);
    }
    REDUCE(MAXPREC,MAXPREC);
}

/* Process an atom. */

LOCAL void SQUASHRAND (A)
    ATOM A;
{ 
    int  P, LP;
    P= oprec(A);
    LP= LPREC(A);
    if (LP<LOPREC ||
        (P>SUBPREC && 
        CONTEXT!=OUTERK && CONTEXT!=INNERK && CONTEXT!=CURLYK))
        SYNERROR(PRECE);
    REDUCE(P,LP);
}

/* Read an atom or string quoted by 'q' and store its characters
      in the atom table, translating pairs of embedded quotes. */

LOCAL void SCANQUOTE (Q)
char Q;
{ 
    STARTATOM();
    for(;;)
    {
        if (CH==Q)
        { 
            GETCHAR(); 
            if (CH!=Q) return; 
        }
        if (CH=='\\')
        { 
            GETCHAR();
            switch(CH)
            { 
            case 'n': CH='\n'; break;
            case 'r': CH='\r'; break;
            case 'b': CH='\b'; break;
            case 't': CH='\t'; break;
            case 'f': CH='\f'; break;
            case 'v': CH='\v'; break;
            case 'a': CH='\007'; break;
            case '\\': CH = '\\'; break;
            case '\n': GETCHAR(); continue;
            default:
                if(!is_digit(CH))
                    break;
                else
                {
                    register unsigned char sum = 0;
                    while(is_digit(CH))
                    {
			sum =  (sum << 3) + (CH -'0');
                        GETCHAR();
                    }
                    ATOMCHAR((char)sum);
                    continue;
                }
            }
        }
        ATOMCHAR(CH); 
        GETCHAR();
    }
}

/* Enter a variable and return it as a term. */

/* variable handling */

LOCAL int VARHWM;
LOCAL TERM ENTERVAR ()
{ 
    TERM  V;
    int  N=0,NEWINDEX;
    static char vtab[VARLIMIT];
    string NEWVAR;
    NEWVAR= &vtab[VARHWM]; 
    NEWINDEX= VARHWM;
    vtab[NEWINDEX++]= LASTCH;
    while (SC<=charclass(CH) && charclass(CH)<=DC)
    { 
        if (NEWINDEX>=VARLIMIT) ABORT(VARSPACEE);
        vtab[NEWINDEX++]= CH;
        GETCHAR();
    }
    vtab[NEWINDEX++]= 0;
    while (N!=VARCT)
    { 
        N++;
        if (strcmp(aIDENT[N-1],NEWVAR)==0){
            counter[N-1]++;
            return VAR_TAB[N-1];
        }
    }
    if (VARCT>=MAXVARS) SYNERROR(NVARSE);
    VARCT++;
    VARHWM= NEWINDEX;
    V= mkfreevar();
    aIDENT[VARCT-1]= NEWVAR;
    VAR_TAB[VARCT-1]= V;
    counter[VARCT-1] = 1;
    return V;
}

LOCAL void Var_Check()
{ 
    int i;
    /* check if a variable is only used only at one time */
    for(i=0;i < VARCT;i++){
        if(counter[i] == 1)
	{
	    START_WARNING();
            ws("WARNING: ");
            ws(aIDENT[i]);
            ws(" used only once ! \n");
	    END_WARNING();
        }
    }
}

#if REALARITH
LOCAL REAL genreal(r,expo)
/* used by readnumber */
/* result is r * 10 ^ expo */
REAL r;
int expo;
{
    while(expo >= 10 ) {
        expo-= 10; 
        r *= 1e10;
    }
    while(expo >= 1){ 
        expo-- ; 
        r *= 10.0; 
    }
    while(expo <= -10 ){
        expo += 10; 
        r *= 1e-10;
    }
    while(expo <= -1) { 
        expo ++ ; 
        r *= 0.1;
    }
    return r;
}
#endif

#if REALARITH && LONGARITH
LOCAL TERM readnumber(sign)
/* read an number - LASTCH is the first digit */
boolean sign;
{
    LONG l ; 
    int  i ; 
    REAL r ;
    boolean esign= false;
    int expo = 0;
    l = (LONG)(i = LASTCH-'0');
    r = (REAL)i;
    while(is_digit(CH)){
        l = l * 10l  + (LONG)(CH -'0');
        i = i * 10   + (CH -'0');
        r = r * 10.0 + (REAL)(CH -'0');
        GETCHAR();
    }
    GETCHAR();
    if(LASTCH == '.' && is_digit(CH))goto double1;
    if((LASTCH == 'e' || LASTCH == 'E') &&
        (is_digit(CH)||CH == '+'||CH =='-'))goto double2;
    /* a normal integer */
    REGET();
    if ( ((REAL)l) <=  r-0.5) {
        /* converting integer to real */
        if((WARNFLAG & WARN_READ) == WARN_READ)
	{
	    START_WARNING();
	    ws("WARNING: converting integer to real\n");
	    END_WARNING();
	}
        return mkreal(sign ? -r : r);
    }
    if ((LONG)i == l){
        /* a normal integer */
        return mkint(sign ? -i : i);
    }
    else {
        /* a long integer */
        return mklong(sign ? -l : l);
    }
double1:
    expo= 0;
    while(is_digit(CH)){
        expo--;
        r = r * 10.0 + (REAL)(CH - '0');
        GETCHAR();
    }
    r = genreal(r,expo);
    GETCHAR();
    if((LASTCH == 'e' || LASTCH == 'E') &&
        (is_digit(CH)||CH == '+'||CH =='-'))goto double2;
    REGET();
    return mkreal( sign ? -r : r);
double2:
    /* scanning the exponent */
    /* exponent starts with CH */
    switch (CH){
    case '-' : 
        esign = true;
    case '+' : 
        GETCHAR();
    }
    expo = 0; /* no exponent => exponent = 0 */
    while(is_digit(CH)){
        if(expo < 1000 )expo = expo * 10 + (CH - '0');
        GETCHAR();
    }
    r = genreal(r, esign ? -expo : expo);
    return mkreal(sign ? -r : r);
}
#endif

#if ! REALARITH && LONGARITH
LOCAL TERM readnumber(sign)
/* read an number - LASTCH is the first digit */
boolean sign;
{
    LONG l ; 
    int  i ;
    l = (LONG)(i = LASTCH-'0');
    while(is_digit(CH))
    {
        l = l * 10l  + (LONG)(CH -'0');
        i = i * 10   + (CH -'0');
        GETCHAR();
    }
    if (i == l) return mkint(sign ? -i : i);
    else return mklong(sign ? -l : l);
}
#endif

#if ! REALARITH && !LONGARITH
LOCAL TERM readnumber(sign)
/* read an number - LASTCH is the first digit */
boolean sign;
{
    int  i ;
    i = LASTCH-'0';
    while(is_digit(CH))
    {
        i = i * 10   + (CH -'0');
        GETCHAR();
    }
    return mkint(sign ? -i : i);
}
#endif

#if REALARITH && !LONGARITH
LOCAL TERM readnumber(sign)
/* read an number - LASTCH is the first digit */
boolean sign;
{
    int  i ; 
    REAL r ;
    boolean esign= false;
    int expo = 0;
    i = LASTCH-'0';
    r = (REAL)i;
    while(is_digit(CH)){
        i = i * 10   + (CH -'0');
        r = r * 10.0 + (REAL)(CH -'0');
        GETCHAR();
    }
    GETCHAR();
    if(LASTCH == '.' && is_digit(CH))goto double1;
    if((LASTCH == 'e' || LASTCH == 'E') &&
        (is_digit(CH)||CH == '+'||CH =='-'))goto double2;
    /* a normal integer */
    REGET();
    if ( ((REAL)i) <=  r-0.5) {
        /* converting integer to real */
        if((WARNFLAG & WARN_READ) == WARN_READ)
	{
	    START_WARNING();
	    ws("WARNING: converting integer to real\n");
	    END_WARNING();
	}
        return mkreal(sign ? -r : r);
    }
    /* a normal integer */
    return mkint(sign ? -i : i);
double1:
    expo= 0;
    while(is_digit(CH)){
        expo--;
        r = r * 10.0 + (REAL)(CH - '0');
        GETCHAR();
    }
    r = genreal(r,expo);
    GETCHAR();
    if((LASTCH == 'e' || LASTCH == 'E') &&
        (is_digit(CH)||CH == '+'||CH =='-'))goto double2;
    REGET();
    return mkreal( sign ? -r : r);
double2:
    /* scanning the exponent */
    /* exponent starts with CH */
    switch (CH){
    case '-' : 
        esign = true;
    case '+' : 
        GETCHAR();
    }
    expo = 0; /* no exponent => exponent = 0 */
    while(is_digit(CH)){
        if(expo < 1000 )expo = expo * 10 + (CH - '0');
        GETCHAR();
    }
    r = genreal(r, esign ? -expo : expo);
    return mkreal(sign ? -r : r);
}
#endif

LOCAL void handle_macros(T)
    TERM T;
{
    TERM Term_Top; 
    TRAIL Trail_Top;
    int ar;
    CLAUSE CL;

    if(clause(MACRO_2) == NIL_CLAUSE)
	return;

    deref(T);

    if(name(T) == UNBOUNDT)
	return;

    if(norm_atom(name(T)))
    {
	TERM S;
	for(ar = arity(name(T)), S = son(T) ; ar > 0 ; --ar, next_br(S))
	    handle_macros(S);
    }

    Term_Top = mark_term_stack(); /* nicht weiter oben ! */
    Trail_Top = mark_trail_stack();

    for(CL = clause(MACRO_2) ; CL != NIL_CLAUSE; CL = nextcl(CL))
    { 
	TERM Base = stackvar(nvars(CL));
	TERM Head = ClauseHead(CL,Base);

	if(UNI(arg1(Head),T))
	{
	    name(T) = name(arg2(Head));
	    son(T) = son(arg2(Head));
	    break;
	}

	rel_term_stack(Term_Top);
	rel_trail_stack(Trail_Top);
    }
}

GLOBAL TERM READIN ()
{ 
    TERM T, X;
    ATOM A,HA,TA,XA;
    int N,TTOP;
    STATE K; 
    int H;
    boolean atom_is_quoted = false;

    RTOP= 0;
    STOP= 0;
    VARHWM= 0;
    VARCT= 0;
    RPUSH(MARKL,NIL_ATOM,NIL_TERM);
    CONTEXT= OUTERK;
    OPEXPECTED= false;
    HIPREC= MAXPREC;
    GETCHAR(); /* next char is CH */
    for (;;)
    { 
        if (FILEENDED())
        { 
            if(RTOP <= 1) 
                return mkatom(END_0);
            ERROR(EOFE);
        }
        atom_is_quoted = false;
        ERRPOS= CHARPOS;
        GETCHAR();

	/* skip first line if it start with #! */
	if( LINENUMBER == 0 && CHARPOS == 2 && LASTCH == '#' && CH == '!')
	{
	    while(CH != '\n')
		GETCHAR();
	    continue;
	}

        switch (charclass(LASTCH))
        { 
        case SC:
            STARTATOM(); 
            ATOMCHAR(LASTCH);
            while ( SC<=charclass(CH) && charclass(CH)<=DC )
            { 
                ATOMCHAR(CH); 
                GETCHAR(); 
            }
            goto new_atom;

        case '\'':
            SCANQUOTE('\''); 
            atom_is_quoted = true;
            goto new_atom;

        case '"':
            SCANQUOTE('"');
            ATOMCHAR(0);
            T= LISTREP(NEWATOM, STRING_CONS, STRING_NIL); 
            TA=name(T); 
            T=son(T); 
            goto shift;

        case OC:
            if (LASTCH=='/' && CH=='*')
            /* A comment. Comments don't nest. */
            {
                GETCHAR();
                do {
                    GETCHAR();
                    if(FILEENDED())ERROR(COMMENTE);
                }            while (LASTCH!='*' || CH!='/');
                GETCHAR();
                ;
                continue;
            }

            if (LASTCH == '%')
            /* also a comment */
            { 
                if(CH != '\n')
                    while(!LINEENDED()) GETCHAR();
                GETCHAR();
                continue;
            }

            if (LASTCH=='-' && charclass(CH)==DC && !OPEXPECTED)
            /* A negative number. */
            { 
                GETCHAR();
                T=readnumber(true); 
                TA=VART; 
                goto shift; 
            }

            if (LASTCH=='.' && is_space(CH))
            /* A full stop. */
            {
                CHECKDELIM();
                if (CONTEXT!=OUTERK) SYNERROR(BADDOTE);
		if((WARNFLAG & WARN_READ) == WARN_READ && 
			     !In_Toplevel_Read)
		    Var_Check();
                RPOP(&TA,&T); 
		T = mkfunc(TA,T);
		handle_macros(T);
		return T;
            }
#if USER_UNIFY
	    if( LASTCH == '<' && CH == '<' )
	    {
		if (OPEXPECTED) SYNERROR(NEEDOPE);
		debug("enter <<");
		RPUSH(MARKL,NIL_ATOM,NIL_TERM);
		SAVECONTEXT();
		CONTEXT= SpezialCK;
		HIPREC= MAXPREC;
		GETCHAR() ; GETCHAR() ; /* skip < & < */
		goto spezial_atom;
	    }
#endif

            STARTATOM(); 
            ATOMCHAR(LASTCH);
            while (charclass(CH)==OC)
            { 
                ATOMCHAR(CH); 
                GETCHAR(); 
            }
            goto new_atom;

        case BC:
            /* An anonymous able: replaced by a unique ordinary able. */
            if (LASTCH=='_' && (charclass(CH)>DC || charclass(CH)==0))
            { 
                TA=UNBOUNDT; 
                T=NIL_TERM; 
            }
	    else /* ordinary variables */
	    {
		T= ENTERVAR(); 
		TA=VART; 
	    }

#if HIGHER_ORDER
	    if(CH == '(') /* VARIABLE AS HIGHER_ORDER_RELATION */
	    {
		/* change to: VAR_CALL ( T ,	*/
		if (OPEXPECTED) 
		    SYNERROR(NEEDOPE);
                GETCHAR(); 
		/* enter: VAR_CALL */
                RPUSH(FUNCL,VAR_CALL,NIL_TERM);
                K= FUNCK; H= SUBPREC;
		/* enter: (        */
		SAVECONTEXT();
		CONTEXT= K;
		HIPREC= H;
		/* enter: Variable */
		RPUSH(TERML,TA,T);
		OPEXPECTED= true;
		LOPREC=0;
		/* enter: comma    */
		goto enter_comma;
	    }
#endif
            goto shift;

        case DC:
            /* positiv numbers */
            T= readnumber(false); 
            TA=VART; 
            goto shift;


        case '(':
            K= INNERK; 
            H= MAXPREC; 
            goto enter;

        case ')':
            CHECKDELIM();
            switch (CONTEXT)
            { 
            case INNERK:
                RPOP(&TA,&T); 
                goto exit_func;
            case FUNCK:
                /* assemble a function call */
                TTOP=RTOP--;
                for (N=1; TAGSTACK[RTOP]==TERML; RTOP--) N++;
                RTOP=TTOP;
                X= stackterm(N); 
                TTOP=N;
                while (N-->0)
                { 
                    TERM ARGS; 
                    ATOM ARGA;
                    RPOP(&ARGA,&ARGS);
                    name(n_brother(X,N))=ARGA; 
                    son(n_brother(X,N))=ARGS; 
                }
                T=X; 
                TA=LOOKATOM(ASTACK[RTOP],TTOP,true);
                goto exit_func;
            default:
                SYNERROR(BADKETE);
            }

        case '!':
            A=CUT_0;
            goto get_atom;

        case '[':
            while(charclass(CH) == C0 && !FILEENDED())GETCHAR();
            if (CH==']') /* The empty list []. */
            { 
                GETCHAR(); 
                A=NIL_0; 
                goto get_atom; 
            }

            K= LISTK; 
            H= SUBPREC; 
            goto enter;

        case ']':
            CHECKDELIM();
#if USER_UNIFY
	    if( CH == ']' &&  CONTEXT == SpezialPK )
		goto exit_spezialP;
#endif
            if (CONTEXT==LISTK)
                RPUSH(TERML,NIL_0,NIL_TERM);
            else if (CONTEXT!=ENDLISTK)
                SYNERROR(BADKETE);
            /* assemble a list */
            RPOP( &TA, &T);
            do { 
                RPOP( &XA, &X); 
                T=mk2sons(XA,X,TA,T); 
                TA=CONS_2; 
            }        while (TAGSTACK[RTOP]==TERML);
            goto exit_func;

        case '{':
            while(charclass(CH) == C0 && !FILEENDED())GETCHAR();
            if (CH=='}') /* The 'curly' atom. */
            { 
                GETCHAR(); 
                A= CURLY_0; 
                goto get_atom; 
            }

            K=CURLYK; 
            H=MAXPREC; 
            goto enter;

        case '}':
            CHECKDELIM();
            if (CONTEXT!=CURLYK) SYNERROR(BADKETE);
            RPOP(&TA,&T); 
            T=mkfunc(TA,T); 
            TA=CURLY_1; 
            goto exit_func;

        case ',':
            if (CH=='.') /* Hope to find ',..', which is punned to '|'. */
            { 
                GETCHAR();
                if (CH!='.') SYNERROR(BADCDDE);
                CH= '|'; 
                continue;
            }

#if HIGHER_ORDER
    enter_comma:
#endif
            switch(CONTEXT)
            { 
            case OUTERK: 
            case INNERK: 
            case CURLYK:
                A= COMMA_2; 
                goto get_atom;
            case FUNCK: 
            case LISTK:
                CHECKDELIM();
                OPEXPECTED= false;
                HIPREC= SUBPREC;
                continue;
            default:
                SYNERROR(BADCDDE);
            }


        case '|':
	    if(CH == '|')
	    {
		STARTATOM(); 
		ATOMCHAR('|');
		ATOMCHAR('|');
		GETCHAR();
		goto new_atom;
	    }
            CHECKDELIM();
            if (CONTEXT!=LISTK) SYNERROR(BADCDDE);
            CONTEXT= ENDLISTK;
            OPEXPECTED= false;
            HIPREC= SUBPREC;
            continue;

        case C0:
	case  0:
            continue;

        default:
	    SYSTEMERROR("READIN.823");
        }
        /* semantic actions */
new_atom:
        ATOMCHAR(0);
        A= LOOKUP(NEWATOM,0,false,true);
get_atom:
        if (!OPEXPECTED)
        {
            if (CH=='(') /* functor in standard notation. */
            { 
                GETCHAR(); 
                RPUSH(FUNCL,A,NIL_TERM);
                K= FUNCK; 
                H= SUBPREC;
                goto enterfunc;
            }
            if(atom_is_quoted) goto quot_atom;
            if((HA = LOOKATOM(A,1,false)) != NIL_ATOM  && 
               (oclass(HA)==FXO || oclass(HA)==FYO))
            { 
                A = HA;
                if (oprec(A)>HIPREC) SYNERROR(PRECE);
                RPUSH(OPL,A,NIL_TERM);
                OPEXPECTED= false; 
                HIPREC= RPREC(A);
                continue;
            }
            /* An atom, i.e. a functor of tarity 0. */
quot_atom:
            A = LOOKATOM(A,0,true);
            RPUSH(TERML,A,NIL_TERM);
            OPEXPECTED= true; 
            LOPREC= 0;
            continue;
        }
        /* OPEXPECTED ! */
        if(atom_is_quoted) SYNERROR(NEEDOPE);
        if((HA = LOOKATOM(A,1,false)) != NIL_ATOM  && 
            (oclass(HA)==XFO || oclass(HA)==YFO))
        {  
            TERM Y; 
            ATOM YA;
            A = HA;
            SQUASHRAND(A);
            RPOP(&YA,&Y);
            Y=mkfunc(YA,Y);
            RPUSH(TERML,A,Y);
            OPEXPECTED= true; 
            LOPREC= oprec(A);
            continue;
        }
        if((HA = LOOKATOM(A,2,false)) != NIL_ATOM  && 
            (oclass(HA)==XFXO || oclass(HA)==XFYO || oclass(HA)==YFXO))
        {  
            A = HA;
            SQUASHRAND(A);
            RPUSH(OPL,A,NIL_TERM);
            OPEXPECTED= false; 
            HIPREC= RPREC(A);
            continue;
        }
        else 
            SYNERROR(NEEDOPE);

shift:
        if (OPEXPECTED) SYNERROR(NEEDOPE);
        RPUSH(TERML,TA,T);
        OPEXPECTED= true;
        LOPREC=0;
        continue;
enter:
        if (OPEXPECTED) SYNERROR(NEEDOPE);
        RPUSH(MARKL,NIL_ATOM,NIL_TERM);
enterfunc: /* also called for call in standard syntax */
        SAVECONTEXT();
        CONTEXT= K;
        OPEXPECTED= false;
        HIPREC= H;
        continue;
exit_func:
        RTOP--; 
        RPUSH(TERML,TA,T);
        RESTORECONTEXT();
        OPEXPECTED= true;
        LOPREC= 0;
        continue;
#if USER_UNIFY
spezial_atom: /* Start with LASTCH */
	STARTATOM();
	while((LASTCH != '>' || CH != '>') && (LASTCH != '[' || CH != '['))
	{
	    if(FILEENDED())
		ERROR(EOFE);
	    ATOMCHAR(LASTCH);
	    GETCHAR();
	}
	if(LASTCH == '[')
	{
	    ATOMCHAR(LASTCH); ATOMCHAR(CH);
	}
	ATOMCHAR(0);
        A= LOOKUP(NEWATOM,0,false,true);
	RPUSH(MATOML,A,NIL_TERM);
	GETCHAR();
	if( LASTCH == '>' )
	    goto exit_spezialC;
	else
	    goto enter_spezialP;

enter_spezialP:
	debug("enter_spezialP");
        RPUSH(MARKL,NIL_ATOM,NIL_TERM);
	SAVECONTEXT();
	CONTEXT = SpezialPK;
	HIPREC = MAXPREC ;
	OPEXPECTED = false;
	continue;
exit_spezialP:
	debug("exit_spezialP");
	CHECKDELIM();
	if( CONTEXT != SpezialPK )
	    ERROR(0);
	RPOP(&TA, &T);
	RTOP --;
	RPUSH(TERML , TA , T );
	RESTORECONTEXT();
	if( CONTEXT != SpezialCK)
	    ERROR(0);
	goto spezial_atom;
exit_spezialC:
	{
	    int i,j,ar,bottom;
	    STRING S;
	    debug("exit_spezialC");
	    i = RTOP;
	    ar = 0;
	    while(TAGSTACK[i] != MARKL)
		--i;
		    ;
	    bottom = i;

	    STARTATOM();
	    while(++i <= RTOP)
	    {
		if(TAGSTACK[i] == MATOML)
		{
		    A = ASTACK[i];
		    S = atomstring(A);
		    for(j = 0 ; stringchar(S,j) != 0 ; ++j)
			ATOMCHAR(stringchar(S,j));
		}
		else if(TAGSTACK[i] == TERML )
		    ++ar;
	    }
	    ATOMCHAR(0);

	    A = LOOKUP(NEWATOM, ar,false,true);
	    X = stackterm(ar);

	    i = bottom;
	    j = 0;
	    while(++i <= RTOP)
	    {
		if(TAGSTACK[i] == TERML)
		{
		    name(n_brother(X,j))  = ASTACK[i];
		    son(n_brother(X,j))   = TSTACK[i];
		    ++j;
		}
	    }

	    RTOP = bottom;
	    --RTOP;

	    RPUSH(TERML,A,X);

	    {
		TERM H,B;
		CLAUSE C , CL;

		H = mkfunc(UNIFY_2,mk2sons(A,stackvar(arity(A)),
					   UNBOUNDT,NIL_TERM));
		B = mkfunc(SCAN_2 , mk2sons(VART , son(H) , VART , br(son(H))));

		CL = NewClause(H,B,false);
		    nextcl(CL) = NIL_CLAUSE;

		if( (C = clause(UNIFY_2)) == NIL_CLAUSE)
		{
		    clause(UNIFY_2) = CL;
		}
		else
		{
		    while(nextcl(C) != NIL_CLAUSE)
			C = nextcl(C);
		    nextcl(C)  = CL;
		}
    		set_in_use(CL);
	    }
	    RESTORECONTEXT();
	    OPEXPECTED= true;
	    LOPREC= 0;
	}
#endif
        continue;
    }
    /* ReadIn */
}


GLOBAL boolean DOREAD() /* read/2 */
{
    TERM T,TT,Q;
    int i;

    if(!UNI(A0,READIN())) return false;
    TT = T = mkatom(CONS_2);
    for(i = 0; i < VARCT  ; ++i)
    {
        Q = mk2sons(LOOKUP(aIDENT[i],0,false,true),NIL_TERM,VART,VAR_TAB[i]);
        son(T) = mk2sons(ISEQ_2,Q,CONS_2,NIL_TERM);
        T = br(son(T));
    }
    name(T) = NIL_0;
    son(T) = NIL_TERM;
    return UNI(TT,A1);
}


/* end of file */
