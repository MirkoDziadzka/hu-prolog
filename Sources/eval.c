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


#define test(X)
#ifdef not_defined
#undef test
#define test(X) ws("<"),ws(X),ws(">") 
#endif

#include "config.h"
#include "types.h"
#include "atoms.h"
#include "files.h"
#include "extern.h"
#include "paths.h"

#if WINDOWS
extern void t_init();
extern void w_exit();
#endif


#if !NO_TRACE
#define set_spytrace()   (SPYTRACE= (SPYCOUNT || TRACING))
GLOBAL int SPYCOUNT;
#endif


GLOBAL TERM A0,A1,A2;

GLOBAL boolean isatom(T)
    TERM T;
{
    ATOM A;
    return norm_atom(A = name(T)) && arity(A) == 0;
}

GLOBAL void CHECKATOM(T)
    TERM T;
{
    if(!isatom(T))
        ERROR( ARGE );
}

LOCAL boolean DOGETENV()
{
    char *envp;
    if(!isatom(A1))
        ERROR(ARGE);
    if((envp = getenv(tempstring(atomstring(name(A1))))) != (char *)0)
        return UNI(A0,mkatom(LOOKUP(envp,0,false,true)));
    else 
        return false;
}

LOCAL boolean DOPUTENV()
{
    if(isatom(A0) && isatom(A1))
    {
        int i;
        STRING S;
        STARTATOM();
        for(i=0,S=atomstring(name(A0));stringchar(S,i) != '\0';++i)
            ATOMCHAR(stringchar(S,i));
        ATOMCHAR('=');
        for(i=0,S=atomstring(name(A1));stringchar(S,i) != '\0';++i)
            ATOMCHAR(stringchar(S,i));
        ATOMCHAR('\0');
        return setenv(NEWATOM) != (char *)0;
    }
    else 
        ERROR(ARGE);
    /*NOTREACHED*/
    return false;
}


LOCAL boolean DODICT(sys_prad)
    boolean sys_prad;
{
    ATOM A=NIL_ATOM;
    TERM T=NIL_TERM;

    while((A=GetAtom(A)) != NIL_ATOM)
    {
        if ((system(A) && !sys_prad) || (sys_prad && !system(A)))
            continue;
        T=mk2sons(DIVIDE_2,
            mk2sons(LOOKATOM(A,0,true),NIL_TERM,INTT,arity(A)),
            (T != NIL_TERM ? CONS_2:NIL_0),T);
    }
    if( T != NIL_TERM)
        return UNI(A0,mkfunc(CONS_2,T));
    else
        return UNI(A0,mkfunc(NIL_0,T));
}

GLOBAL int compare(A,B,DEPTH)
    TERM A,B;
    int DEPTH;
{
    int N;
    ATOM AA,BB;
#if REALARITH
    REAL R;
#endif
#if LONGARITH
    LONG L;
#endif
    int aar,bar;
#if REALARITH
#define realret(expr)  if ((R=(expr)) < 0.0) return -1;\
                                else return  (R > 0.0)
#endif   
#if LONGARITH
#define longret(expr) if ((L=(expr)) < (LONG)0) return -1;\
                                else return (L > (LONG)0)
#endif
    if (DEPTH <= 0) ABORT(DEPTHE);
    deref(A); 
    deref(B);
    if((AA=name(A))==UNBOUNDT)
        return (name(B) == UNBOUNDT)  ? 0 : -1;
    else if(norm_atom(AA))
    {   
        STRING AIDX,BIDX; 
        char CA,CB;
        int ai,bi;
        if(!norm_atom(BB=name(B)))
            return 1;
        AIDX=atomstring(AA); 
        BIDX=atomstring(BB);
        for(ai=0,bi=0;;++ai,++bi)
        { 
            CA = stringchar(AIDX,ai);
            CB = stringchar(BIDX,bi);
            if(CA != CB)
                return CA-CB;
            else if(CA == 0)
                break;
        }
        aar = arity(AA); 
        bar = arity(BB);
        A=son(A); 
        B=son(B);
        N = 0;
        for(;aar && bar && ((N=compare(A,B,DEPTH-1)) == 0);--aar,--bar)
        {
            next_br(A);
            next_br(B);
        }
        if(N) return N;
        return (aar - bar);
    }
    else if(AA == INTT )
    {
        if((BB=name(B)) == UNBOUNDT) 
            return 1;
        if(BB == INTT) return ival(A)-ival(B);
#if LONGARITH
        else if(BB == LONGT)
            longret((LONG)ival(A)-longval(B));
#endif
#if REALARITH
        else if (BB == REALT)
            realret((REAL)ival(A)-realval(B));
#endif
        else return -1;
    }
#if LONGARITH
    else if (AA == LONGT)
    {
        if((BB=name(B)) == UNBOUNDT) 
            return 1;
        else if(BB == INTT) 
            longret(longval(A)-(LONG)ival(B));
        else if (BB == LONGT)
            longret(longval(A)-longval(B));
#if REALARITH
        else if (BB == REALT)
            realret((REAL)longval(A)-realval(B));
#endif
        else return -1;
    }
#endif
#if REALARITH
    else if (AA == REALT )
    {
        if((BB=name(B)) == UNBOUNDT) return 1;
        else if (BB == INTT) 
            realret(realval(A)-(REAL)ival(B));
#if LONGARITH
        else if (BB == LONGT)
            realret(realval(A)-(REAL)longval(B));
#endif
        else if (BB == REALT)
            realret(realval(A)-realval(B));
        else return -1;
    }
#endif
#undef realret
#undef longret
    else
	SYSTEMERROR("compare");
    /*NOTREACHED*/
    return 0;
}


LOCAL boolean DOFUNCTOR ()
{
    ATOM A;
    int ar;

    A=name(A0);
    if(is_number(A))
        return UNI(A0,A1) && INTRES(A2,0);
    else if(A == UNBOUNDT)
    { 
        ar = INTVALUE(A2);
        if(isatom(A1) && ar >= 0)
            return UNI(A0,
                       mkfunc(LOOKATOM(name(A1),ar,true),stackvar(ar)));
        if(is_number(name(A1)) && ar == 0)
            return UNI(A0,A1);
        ARGERROR();
    }
    else if(norm_atom(A))
    {
#if HIGHER_ORDER
	if(var_call(A))
	{
	    return INTRES(A2,arity(A)-1) &&
		   UNI(A1,arg1(A0));
	}
	else
#endif
	    return INTRES(A2,arity(A)) && 
		   UNI(A1,mkatom(LOOKATOM(A,0,true)));
    }
    else SYSTEMERROR("DOFUNCTOR");
    /*NOTREACHED*/
    return false;
}

LOCAL boolean DOARG ()
{
    register int N;
    if (name(A0) != INTT || !norm_atom(name(A1))) 
        ABORT(ARGE);
    N= ival(A0);
#if HIGHER_ORDER
    if(var_call(name(A1)))
    {
	if (N<1 || N >= arity(name(A1)))
	    ABORT(ARGE);
	return UNI(A2,n_brother(son(A1),N));
    }
    else
#endif
   {
	if (N<1 || N > arity(name(A1)))
	    ABORT(ARGE);
	return UNI(A2,n_brother(son(A1),N-1));
   }
}

LOCAL void LISTCLAUSE(CL)
    CLAUSE CL;
{
    register TERM T;
    register int notfirst = 0;
    LISTOUT(head(CL));
    T = body(CL);
    for(;;)
    {
        while(name(T) == GOTOT) 
            T = son(T);
        if(name(T) == NIL_ATOM) 
            break;
        ws( notfirst++ ? ",\n  " : " :-\n  ");
        LISTOUT(T);
        next_br(T);
        if(UserAbort) 
            break;
    }
    ws(".\n");
}

LOCAL int LISTATOM(A)
    ATOM A;
{
    register CLAUSE CL; 
    register int N;

    if(class(A) != NORMP || hide(A) || private(A)) 
        return 0;
    if(lib(A) && !DEBUGFLAG)
	return 0;
    if((system(A) && !DEBUGFLAG) || A == REPEAT_0)
	return 0;
    if(stringchar(atomstring(A),0)== '$' && !DEBUGFLAG) 
        return 0;
    CL=clause(A);
    for(N = 0; CL != NIL_CLAUSE && !UserAbort;CL = nextcl(CL),++N)
        LISTCLAUSE(CL);
    return N;
}

LOCAL void DOLISTING()
{
    ATOM A,AA;
    int N=0,i;

    if(isatom(A0))
    {
        if((A = name(A0)) != ALL_0 )
        {   
            for(i=0;i<= MAXARITY;++i)
            { 
                AA = LOOKATOM(A,i,false);
                if(AA != NIL_ATOM && is_heap_atom(AA)) 
                    N += LISTATOM(AA);
                if(UserAbort) return;
            }
            if ((WARNFLAG & WARN_LISTING) == WARN_LISTING && N==0 )
            { 
		START_WARNING();
                ws("WARNING: no clause for relation ");
                wq(name(A0)); 
                ws("\n");
		END_WARNING();
            }
        }
        else 
        { /* name(A0) == ALL_0 */
            A= NIL_ATOM;
            while((A=GetAtom(A)) != NIL_ATOM)
            {
                N+=LISTATOM(A);
                if(UserAbort) return;
            }
        }
    }
    else  /* ! isatom(A) */
    {
        if(is_heap_atom(A = atom(A0)))
            N=LISTATOM(A);
        if ((WARNFLAG & WARN_LISTING) == WARN_LISTING && N==0)
        {
	    START_WARNING();
            ws("WARNING: no clause for relation ");
            wq(A);
            ws("/");
            ws(itoa(arity(A))); 
            ws("\n");
	    END_WARNING();
        }
    }
}

#if !NO_TRACE

LOCAL void DOSPY(SPYMODE)
    boolean SPYMODE;
{
    ATOM A;
    int i;
    if(isatom(A0))
    {
        if(name(A0) != ALL_0)
            for(i= 0;i<= MAXARITY;i++)
            {
                if((A= LOOKATOM(name(A0),i,false)) != NIL_ATOM)
                {
                    A= copyatom(A);
                    if(!spy(A) && SPYMODE) SPYCOUNT++;
                    if(spy(A) && !SPYMODE) SPYCOUNT--;
                    if(SPYMODE) setspy(A); 
                    else setnotspy(A);
                }
            }
        else /* name(A0) == ALL_0 */
        {
            A= NIL_ATOM;
            while((A= GetAtom(A)) != NIL_ATOM)
            {
                if(!spy(A) && SPYMODE) SPYCOUNT++;
                if(spy(A) && !SPYMODE) SPYCOUNT--;
                if (SPYMODE) setspy(A); 
                else setnotspy(A);
            }
        }
    }
    else
    {
        A=atom(A0);
        if ((WARNFLAG & WARN_SPY) == WARN_SPY  && 
	     clause(A) == NIL_CLAUSE)
        {
	    START_WARNING();
            ws("WARNING: no clause for relation ");
            wq(A); ws("/"); ws(itoa(arity(A)));
	    END_WARNING();
        }
        A=copyatom(A);
        if(!spy(A) && SPYMODE) 
            SPYCOUNT++;
        if(spy(A) && !SPYMODE) 
            SPYCOUNT--;
        if(SPYMODE) 
            setspy(A); 
        else 
            setnotspy(A);
    }
    set_spytrace();
}

#endif	/* !NO_TRACE */

GLOBAL boolean DOEQUAL(X0,X1,Depth)
    TERM X0,X1;
    int Depth;
{   
    int ar;
    ATOM A;

    if(Depth == 0) ABORT(DEPTHE);
    deref(X0); 
    deref(X1);
    if((A=name(X1))!=name(X0)) 
        return false;
    if(A==INTT) 
        return ival(X0)==ival(X1);
    if(A==UNBOUNDT) 
        return X0==X1;
    ar = arity(A);
    for(X1=son(X1),X0=son(X0);ar;--ar,next_br(X1),next_br(X0))
        if(! DOEQUAL(X0,X1,Depth-1)) return false;
    return true;
}

GLOBAL boolean ground(ARG,Depth)
    TERM ARG;
    int Depth;
{   
    int ar;
    ATOM A;
    deref(ARG);
    if(Depth == 0) 
        ABORT(DEPTHE);
    if((A=name(ARG))==UNBOUNDT) 
        return false;
    if(!norm_atom(A))
        return true;
    ar= arity(A);
    ARG = son(ARG);
    while(ar-- > 0)
    {
        if(! ground(ARG,Depth-1)) return false;
        next_br(ARG);
    }
    return true;
}

LOCAL boolean DOUNIV()
{
    ATOM A;
    TERM X,Y,T;
    int ar;
    A=name(A0);
    if (A==UNBOUNDT)
    {  
        for(T= A1,ar= -1; name(T)==CONS_2; ++ar) 
            T=arg2(T);
        if(name(T) != NIL_0 || ar < 0) 
            ARGERROR();
        if(ar==0) 
            return UNI(A0,son(A1));
        T=arg1(A1);
        CHECKATOM(T);
        Y=mkfunc(LOOKATOM(name(T),ar,true),(X = stackvar(ar)));
        T=arg2(A1);
        while(ar--> 0)
        { 
            (void)UNI(X,son(T));
            next_br(X); 
            T = arg2(T);
        }
        return UNI(Y,A0);
    }
    if (!norm_atom(A))
        return UNI(A1,mkfunc(CONS_2,mk2sons(A,son(A0),NIL_0,NIL_TERM)));
    /* default */
    ar= arity(A);
    T= son(A0);
#if HIGHER_ORDER
    if(var_call(A))
    {
	X=Y=mkfunc(CONS_2,mk2sons(UNBOUNDT,NIL_TERM,
				  NIL_0,NIL_TERM));
	UNI(son(X), T);
	next_br(T); --ar;
    }
    else
#endif
	X=Y=mkfunc(CONS_2,mk2sons(LOOKATOM(A,0,true),NIL_TERM,
				  NIL_0,NIL_TERM));

    while (ar-->0)
    { 
        Y=br(son(Y));
        name(Y)=CONS_2; 
        son(Y)=mk2sons(UNBOUNDT,NIL_TERM,NIL_0,NIL_TERM);
        (void)UNI(son(Y),T);
        next_br(T);
    }
    return UNI(X,A1);
}  /* univ */


LOCAL void SetFlag(FLAG)
    boolean *FLAG;
{ 
    ATOM A;
    A=name(A0);
    if(A==ON_0) *FLAG=true;
    else if (A==OFF_0) *FLAG=false;
    else if (A==UNBOUNDT)
        if (*FLAG)  (void)UNI(A0,mkatom(ON_0));
        else (void)UNI(A0,mkatom(OFF_0));
    else ARGERROR();
}

LOCAL boolean DOCURATOM( curr_pred )
    boolean curr_pred; 
{
    ATOM A;
    TERM T;
    TRAIL Trail_Top;
    TERM  Term_Top;

    Trail_Top = mark_trail_stack();
    Term_Top = mark_term_stack();

    /* unsauber programmiert, geht aber nicht besser */
    /* evtl. if(BCT != ..) A = name(BCT);            */
    /*        BCT = mkatom(A)                        */
    while((BCT=(TERM)(A=GetAtom((ATOM)BCT))) != (TERM)NIL_ATOM)
    {
        if(curr_pred && class(A) != EVALP && class(A) != BTEVALP &&
	  (class(A) != NORMP || clause(A) == NIL_CLAUSE ))
	    continue;
        T=mkfunc(DIVIDE_2,
                 mk2sons(LOOKATOM(A,0,true),NIL_TERM,INTT,arity(A)));
        if(UNI(T,A0))
            return true;

        rel_term_stack(Term_Top);
        rel_trail_stack(Trail_Top);
    }
    return false;
}


LOCAL boolean DOCUROP()
{
    ATOM A,P;

    TRAIL Trail_Top;
    TERM  Term_Top;

    Trail_Top = mark_trail_stack();
    Term_Top = mark_term_stack();

    /* siehe DOCURATOM */
    while((BCT=(TERM)(A=GetAtom((ATOM)BCT))) != (TERM)NIL_ATOM)
    {
        switch(oclass(A))
        {
        case XFO:   P = XF_0;  break;
        case FXO:   P = FX_0;  break;
        case XFXO:  P = XFX_0; break;
        case YFO:   P = YF_0;  break;
        case FYO:   P = FY_0;  break;
        case XFYO:  P = XFY_0; break;
        case YFXO:  P = YFX_0; break;
        default:    continue;
        }
        if(UNI(A0,mkint(oprec(A))) && UNI(A1,mkatom(P))   &&
                UNI(A2,mkatom(LOOKATOM(A,0,true))))
            return true; 

        rel_term_stack(Term_Top);
        rel_trail_stack(Trail_Top);
    }
    return false;
}

LOCAL void copy(s)
    char *s;
{
    while(*s)
        ATOMCHAR(*s++);
    ATOMCHAR(' ');
}


LOCAL boolean DOANCESTORS(ENVP)
    ENV ENVP;
{
    TERM T,TT,C;
    ENV CE;

    TT=mkfunc(CONS_2,mk2sons(UNBOUNDT,NIL_TERM,NIL_0,NIL_TERM)); 
    T=TT;
    for(CE=ENVP; CE != NIL_ENV ; CE=env(CE))
        if((C=call(CE)) != NIL_TERM && name(C)!=SEMI_2 
            && name(C)!=COMMA_2)
        {   
            if(stringchar(atomstring(name(C)),0) == '$')
                continue;
            T=son(T);
            (void)UNIFY(1,T,C,BE,base(env(CE)),MAXDEPTH);
            next_br(T);
            name(T)=CONS_2;
            son(T)=mk2sons(UNBOUNDT,NIL_TERM,NIL_0,NIL_TERM);
        }
    name(T)=NIL_0; 
    son(T)=NIL_TERM;
    return UNI(A0,TT);
}

LOCAL boolean DOSYSTEM()
{
    TERM T;
    ATOM A;
    boolean res;

    STARTATOM(); /* init NEWATOM */
    while(name(A0) == CONS_2)
    {
        T= arg1(A0); 
        A=name(T); 
        A0= arg2(A0);
        if (A == INTT) copy(itoa(ival(T)));
#if LONGARITH
        else if (A == LONGT) copy(ltoa(longval(T)));
#endif
#if REALARITH
        else if (A == REALT) copy(ftoa(realval(T)));
#endif
        else 
        { 
            CHECKATOM(T); 
            copy(tempstring(atomstring(name(T)))); 
        }
    }
    if(name(A0) != NIL_0)
        ERROR(ARGE);
    ATOMCHAR('\0'); /* terminate NEWATOM */
#if WINDOWS
    if(xWINDOW_ON)
	w_exit();
#endif
    res = (call_system(NEWATOM) != 0);
#if WINDOWS
    if(xWINDOW_ON)
    {
	t_init();
	w_scr_refresh();
    }
#endif
    return res;
}

#if SAVE

LOCAL boolean DOSAVE(save)
    boolean save;
{
    string filename;

    if(!isatom(A0))
	ERROR(ARGE);

    filename = tempstring(atomstring(name(A0)));
    if(save)
    {
	ERRORTYPE result;
	result = save_restore(true,filename);
	if(result != NOERROR)
	    ERROR(result);
	return true;
    }
    /* ! save */
#if WINDOWS
    if(xWINDOW_ON)
	w_exit();
#endif
    (void)execlp(ARGV[0],ARGV[0],"-r",filename,(char *)0);
    /*NOTREACHED*/
    return false;
}
#endif

void SetWarnFlag()
{
    if(name(A0) == UNBOUNDT)
    {
	TERM T,TT;
	T = TT = mkatom(NIL_0);
#define SETWARN(BITMASK,ATOM)					\
	if((WARNFLAG & BITMASK) == BITMASK)                     \
	{ 							\
	    name(T) = CONS_2; 					\
	    son(T)  = mk2sons(ATOM,NIL_TERM,NIL_0,NIL_TERM);    \
	    T = br(son(T)); 					\
	}
	SETWARN(WARN_OP,OP_0)
	SETWARN(WARN_RETRACT,RETRACT_0)
	SETWARN(WARN_CONSULT,CONSULT_0)
	SETWARN(WARN_LISTING,LISTING_0)
#if !NO_TRACE
	SETWARN(WARN_SPY,SPY_0)
#endif
	SETWARN(WARN_READ,READ_0)
	SETWARN(WARN_ASSIGN,ASSIGN_0)
#undef SETWARN
		
	(void)UNI(TT,A0);
    }
    else if(name(A0) == ON_0)
	WARNFLAG = WARN_ON;
    else if(name(A0) == OFF_0)
	WARNFLAG = WARN_OFF;
    else
    {
	card TmpWarnFlag = WARN_OFF;
	if(!islist(A0,false,false))
	    ERROR(ARGE);
	for( ; name(A0) == CONS_2 ; A0 = arg2(A0))
	    switch(name(arg1(A0)))
	    {
		case OP_0:       TmpWarnFlag |= WARN_OP; break;
		case RETRACT_0:  TmpWarnFlag |= WARN_RETRACT; break;
		case CONSULT_0:  TmpWarnFlag |= WARN_CONSULT; break;
		case LISTING_0:  TmpWarnFlag |= WARN_LISTING; break;
#if !NO_TRACE
		case SPY_0:      TmpWarnFlag |= WARN_SPY; break;
#endif
		case READ_0:     TmpWarnFlag |= WARN_READ; break;
		case ASSIGN_0:   TmpWarnFlag |= WARN_ASSIGN; break;
		default:	 ERROR(ARGE);
	    }
	WARNFLAG = TmpWarnFlag;
    }
}

GLOBAL TERM CALLX;
GLOBAL ENV CALLXENV;


GLOBAL boolean CALLEVALPRED (X,ENVP)
    TERM X; 
    ENV ENVP;
{
    extern jmp_buf error_label;
    ATOM NAME;
    int  ar;
    auto jmp_buf old_error_label;
    boolean RES = true;
    CALLX= X; 
    CALLXENV=ENVP;

    SAVE_ERROR((char *)old_error_label);

    if(CATCH_ERROR())
    {
        RES = false;
        test("-");
        goto exit_func;
    }
    test("+");

    E=ENVP; 
    BE=base(ENVP);
    deref(X);
    NAME=name(X);
    ar= arity(NAME);
    if (1<=ar)
    {   
        X= son(X); A0=X; deref(A0);
        if (2<=ar)
        {   
            next_br(X); A1=X; deref(A1);
            if (3<=ar)
            {   
                next_br(X); A2=X; deref(A2);
                if (4 <= ar) SYSTEMERROR("CALLEVALPRED.1");
            }   
        }   
    }

    switch (NAME)
    {
        /* date/time handling */
    case TIME_3:
    case DATE_3:
    case WEEKDAY_1:     RES= DOTIME(NAME); break;
    case TIMER_1:       RES= DOTIMER(); break;

    /* input/output predicates */
    case OP_3:           DOOP(); break;
    case READ_1:        RES = UNI(READIN(),A0); break;
    case READ_2:        RES = DOREAD(); break;
    case WRITE_1:       WRITEOUT(A0,false); break;
    case WRITEQ_1:      WRITEOUT(A0,true); break;
    case DISPLAY_1:     DISPLAY(A0); break;
    case GET0_1:        RES= DOGET0(); break;
    case GET_1:         RES= DOGET(); break;
    case UNGET_0:       REGET(); break;
    case SKIP_1:        DOSKIP(); break;
    case PUT_1:         DOPUT(); break;
    case CLS_0:         DOCLS(); break;
    case NL_0:          ws("\n"); break;
    case TAB_1:         DOTAB(); break;
    case GOTOXY_2:      RES=DOGOTOXY(); break;
    case ASK_1:         RES= DOASK(); break;

    case TTYREAD_1:
    case TTYGET0_1:
    case TTYGET_1:
    case TTYSKIP_1:
    case TTYASK_1:      
        setinfile();
        switch(NAME)
        {
        case TTYREAD_1:  RES = UNI(READIN(),A0); break;
        case TTYGET0_1:  RES= DOGET0(); break;
        case TTYGET_1:   RES= DOGET(); break;
        case TTYSKIP_1:  DOSKIP(); break;
        case TTYASK_1:   RES= DOASK(); break;
        }
        getinfile();
        break;
    case TTYWRITE_1:
    case TTYPUT_1:
    case TTYCLS_0:
    case TTYNL_0:
    case TTYTAB_1:
    case TTYGOTOXY_2:   
        setoutfile();
        switch(NAME)
        {
        case TTYWRITE_1:  WRITEOUT(A0,false); break;
        case TTYPUT_1:    DOPUT(); break;
        case TTYCLS_0:    DOCLS(); break;
        case TTYNL_0:     ws("\n"); break;
        case TTYTAB_1:    DOTAB(); break;
        case TTYGOTOXY_2: RES=DOGOTOXY(); break;
        }
        getoutfile();
        break;

    case T_LINES_1:	RES=INTRES(A0,t_lines()); break;
    case T_COLUMNS_1:   RES=INTRES(A0,t_columns()); break;
    case EOLN_0:        RES=LINEENDED(); break;
    case EOF_0:         RES=FILEENDED(); break;
    case SEE_1:         RES=DOSEE(); break;
    case SEEING_1:      RES=UNI(A0,mkatom(FLOGNAME(inputfile))); break;
    case SEEN_0:        CloseFile(inputfile); A0=mkatom(USER_0);
                        RES=DOSEE(); break;
    case TELL_1:        RES=DOTELL(); break;
    case TELLING_1:     RES=UNI(A0,mkatom(FLOGNAME(outputfile))); break;
    case TOLD_0:        CloseFile(outputfile); A0=mkatom(USER_0);
                        RES=DOTELL(); break;
    case OPEN_1:        RES=DOOPEN(); break;
    case CLOSE_1:       RES=DOCLOSE(); break;
    case SEEK_2:        RES=DOSEEK(); break;
    case WGET0_1:
#if WINDOWS
                        if(xWINDOW_ON) RES = INTRES(A0,t_rc());
                        else 
#endif
                        RES = DOGET0(); break;
    case aWINDOW_0:     RES = xWINDOW_ON; break;
    case ddINIT_W_0:    RES = DOWINDOW(); break ; 
    case FILEE_1:       SetFlag(&FERRORFLAG); break;
    case CONSULT_1:     RES=DOCONSULT(A0,false,false); break;
    case RECONSULT_1:   RES=DOCONSULT(A0,true,false); break;

    case FASSIGN_2:     RES=DOFASSIGN(); break;
    case SYSTEM_1:      RES=DOSYSTEM(); break;
    case ARGC_1:        RES=INTRES(A0,ARGC); break;
    case ARGV_2:        
        {
            int i;
            if((i = INTVALUE(A1))<0 || i >=ARGC)
                ERROR(ARGE);
            RES = UNI(A0,mkatom(LOOKUP(ARGV[i],0,false,true)));
        }
        break;
    case RESTART_0:     ABORT(NOERROR); break;
    case ABORT_0:       ABORT(ABORTE); break;
    case ABORT_2:       CALLX = A0;ABORT(INTVALUE(A1)); break;
    case END_0:         HALTFLAG=true; break;
#if !NO_TRACE
    case TRACE_1:       SetFlag(&TRACING); set_spytrace();
                        TRACE_GOON = 0; break;
    case TRACE_0:       
                        TRACING=true; set_spytrace();
                        TRACE_GOON = 0; break;
    case NOTRACE_0:     
                        TRACING=false; set_spytrace();
                        TRACE_GOON = 0; break;
    case SPY_1:         DOSPY(true); TRACE_GOON = 0; break;
    case NOSPY_1:       DOSPY(false); TRACE_GOON = 0; break;
#endif
    case ECHO_1:        SetFlag(&ECHOFLAG); break;
    case DEBUG_1:       SetFlag(&DEBUGFLAG); break;
    case UUNIFY_1:	SetFlag(&USER_UNIFY_FLAG) ; break;
    case STATS_0:       DOSTATS(); break;
    case LOG_1:         SetFlag(&PROTFLAG); break;
    case SYSMODE_1:     SetFlag(&aSYSMODE); break;
#if OCCUR_CHECK 
    case OCHECK_1:      SetFlag(&OCHECK); break;
#endif
    case WARN_1:        SetWarnFlag(); break;
    case HALT_0:        EXIT_JMP(0); break;
    case EXIT_1:        EXIT_JMP(INTVALUE(A0)); break;

        /* term manipulation */
    case ATOM_1:        RES= isatom(A0); break;
    case LIST_1:        RES= islist(A0, false, false); break;
    case STRING_1:      RES= islist(A0, true, false); break;
    case COMPOUND_1:    
        RES= norm_atom(name(A0)) && arity(name(A0));
        break;
    case INTEGER_1:     RES= is_integer(name(A0)); break;
#if REALARITH
    case REAL_1:        RES= name(A0)==REALT; break;
#endif
    case NUMBER_1:      RES= is_number(name(A0)); break;
    case ATOMIC_1:      
        RES=isatom(A0) || is_number(name(A0));
        break;
    case VAR_1:         RES= name(A0)==UNBOUNDT; break;
    case NONVAR_1:      RES= name(A0)!=UNBOUNDT; break;
    case SYS_1:         RES= system(atom(A0)); break;
    case CURATOM_1:     RES= DOCURATOM( false ); break;
    case CURPRED_1:     RES= DOCURATOM( true ); break;
    case CUROP_3:       RES= DOCUROP(); break;
    case NAME_2:        RES= DONAME();  break;
    case FUNCTOR_3:     RES= DOFUNCTOR();  break;
    case ARG_3:         RES= DOARG();  break;
    case UNIV_2:        RES= DOUNIV(); break;
    case EQUAL_2:       RES= DOEQUAL(A0,A1,MAXDEPTH); break;
    case NOEQUAL_2:     RES= !DOEQUAL(A0,A1,MAXDEPTH); break;
    case INVAR_1:       RES= !ground(A0,MAXDEPTH);break;
    case GROUND_1:      RES= ground(A0,MAXDEPTH); break;
    case ALT_2:         RES= (compare(A0,A1,MAXDEPTH)<0); break;
    case ALE_2:         RES= (compare(A0,A1,MAXDEPTH)<=0); break;
    case AGT_2:         RES= (compare(A0,A1,MAXDEPTH)>0); break;
    case AGE_2:         RES= (compare(A0,A1,MAXDEPTH)>=0); break;
    case AEQ_2:         RES= (compare(A0,A1,MAXDEPTH)==0); break;
    case ANE_2:         RES= (compare(A0,A1,MAXDEPTH)!=0); break;

        /* arithmetic operations, see arith.c */
    case IS_2:          RES= DOIS(); break;
    case ACOMP_1:       RES= DOACOMP(); break;
    case DASSIGN_2:     RES= DODASS(); break;
    case REDUCE_2:      RES = DOREDUCE(A0,A1,false); break;

        /* database operations */
    case DICT_1:        RES=DODICT(false); break;
    case SDICT_1:       RES=DODICT(true); break;

    case ASSERTA_1:     DOASSERT(true); break;
    case ARROW_2:       A0 = mkfunc(ARROW_2,
                        mk2sons(name(A0),son(A0), name(A1),son(A1)));
    case ASSERT_1:
    case ASSERTZ_1:     DOASSERT(false); break;
    case LISTING_0:     A0 = mkatom(ALL_0);/* no break */
    case LISTING_1:     DOLISTING(); break;
    case CLAUSE_2:      RES=DOCLAUSE(); break;
    case RETRACT_1:     RES=DORETRACT(false); break;
    case ABOL_1:        DOABOLISH(1); break;
    case ABOL_2:        DOABOLISH(2); break;
    case HELP_0: 	RES = DOHELP(NIL_ATOM); break;
    case HELP_1:        if(norm_atom(name(A0)))
			  RES = DOHELP(name(A0)); 
		        else
			  ERROR(ARGE);
		        break;
    case VERSION_0:     print_copyright();
	{
	    ws("\nsystem:\t\t");
		    if(POSIX) ws(" POSIX");
		    if(UNIX) ws(" UNIX");
		    if(VMS) ws(" VMS");
		    if(MS_DOS) ws(" MS_DOS");
		    if(BIT16) ws(" BIT16");
		    if(BIT32) ws(" BIT32");
		    if(BYTEMODE) ws(" BYTEMODE");
		    if(WORDMODE) ws(" WORDMODE");
		    if(POINTERMODE) ws(" POINTERMODE");
	    ws("\nlimits:\t");
		    ws("\t MAXDEPTH\t= ");ws(itoa(MAXDEPTH));
		    ws("\n\t\t MAX_NAME_LEN\t= ");ws(itoa(MAX_NAME_LEN));
		    ws("\n\t\t MAXARITY\t= ");ws(itoa(MAXARITY));
		    ws("\n\t\t MAXPREC\t= ");ws(itoa(MAXPREC));
		    ws("\n\t\t MAXVARS\t= ");ws(itoa(MAXVARS));
		    ws("\n\t\t VARLIMIT\t= ");ws(itoa(VARLIMIT));
		    ws("\n\t\t WRITEDEPTH\t= ");ws(itoa(WRITEDEPTH));
		    ws("\n\t\t WRITELENGTH\t= ");ws(itoa(WRITELENGTH));
		    ws("\n\t\t READSIZE\t= ");ws(itoa(READSIZE));
		    ws("\n\t\t READDEPTH\t= ");ws(itoa(READDEPTH));
	    ws("\narithmetik:\t");
		    if(LONGARITH) ws(" LONGARITH");
		    if(REALARITH) ws(" REALARITH");
		    if(FUNARITH) ws(" FUNARITH");
	    ws("\nother options:\t");
		    if(OCCUR_CHECK) ws(" OCCUR_CHECK");
		    if(WINDOWS) ws(" WINDOWS");
		    if(USER) ws(" USER");
		    if(QUICK_BUT_DIRTY) ws(" QUICK_BUT_DIRTY");
		    if(SAVE) ws(" SAVE");
		    if(NO_TRACE) ws(" NO_TRACE");
		    if(EXTENDED_ASCII) ws(" EXTENDED_ASCII");
		    if(LATIN1) ws(" LATIN1");
		    if(DEBUG)  ws(" DEBUG");
	    ws("\nspecials:\t");
		    if(HIGHER_ORDER) ws(" HIGHER_ORDER");
	    ws("\npaths:\t");
#ifdef STDPLGRC
		    ws("\t STDPLGRC\t= ");ws(STDPLGRC);
#endif
#ifdef STD_HELP_FILE
		    ws("\n\t\t STD_HELP_FILE\t= ");ws(STD_HELP_FILE);
#endif
	    ws("\n");
	}
        break;

    case PRIVATE_1:     DOPRIVATE(); break;
    case HIDE_1:        DOHIDE(); break;
    case ANCESTORS_1:   RES = DOANCESTORS(ENVP); break;
    case ENSURE_3:      RES = DOENSURE(); break;

    case GETENV_2:      RES = DOGETENV(); break;
    case PUTENV_2:      RES = DOPUTENV(); break;

#if SAVE
    case SAVE_1:	RES = DOSAVE(true); break;
    case RESTORE_1:	RES = DOSAVE(false); break;
#endif

    default:
#if USER
        RES= CallUser(CALLX,ENVP);
        break;
#endif
#if !USER
        ws("\007sorry, this predicate is reserved, ");
        ws("but not yet implemented\n");
        ERROR(CALLE);
#endif
    }
    CALLX=NIL_TERM;
exit_func:
    RESTORE_ERROR((char *)old_error_label);
    return RES;

}


/* end of file */
