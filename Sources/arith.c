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

#if REALARITH
#       include <math.h>
#endif

#include "types.h"
#include "atoms.h"
#include "extern.h"

#define OPTIMIZE        1

#if REALARITH
#define PI_CONST           3.1415926535
#define E_CONST            2.7182818284
#endif

#define test(p,t)
#ifdef TEST_ARITH
#undef test
#define test(p,t)  if(DEBUGFLAG) \
               {ws("[");ws(itoa(E-MIN_ENV));ws("]");ws(p); \
                WRITEOUT(t,false);ws("\n");}
#endif


#if !LONGARITH
#define LONG int
#define LONGRES(X,Y)    INTRES(X,Y)
#endif

#if REALARITH
typedef struct ATP 
{ 
    boolean atp_isreal;
    union 
    { 
        REAL atp_rrec; 
        LONG atp_lrec; 
    } atp_value;
} ATP_TYPE;

#define realf(av)          ((av)->atp_value.atp_rrec)
#define longf(av)          ((av)->atp_value.atp_lrec)
#define isreal(av)         ((av)->atp_isreal)

LOCAL LONG lis(AT) 
    ATP_TYPE *AT;
{ 
    if(isreal(AT))
        ERROR(STDFUNCARGE);
    return longf(AT); 
}

LOCAL REAL ris(AT) 
    ATP_TYPE *AT;
{ 
    return (isreal(AT) ? realf(AT) : (REAL)longf(AT));
}
#endif
#if !REALARITH
typedef LONG ATP_TYPE;
#define longf(av)       (*(av))
#define lis(av)         (longf(av)+0)
#endif

LOCAL boolean assign(ap,t) 
    ATP_TYPE *ap; 
    TERM t;
{ 
    ATOM A;
    A=name(t);

#if REALARITH
    isreal(ap) = false;
#endif
    if(A==INTT) 
        longf(ap)= (LONG)ival(t);
#if LONGARITH
    else if(A==LONGT) 
        longf(ap)= longval(t);
#endif
#if REALARITH
    else if(A==REALT)
    { 
        realf(ap)= realval(t);
        isreal(ap) = true;
    }
#endif
    else 
        return false;
    return true;
}

LOCAL boolean compute(A,at1,at2,ap,stdfunc)
    ATOM A;
    ATP_TYPE *at1,*at2,*ap;
    boolean stdfunc;
{
#if REALARITH
    isreal(ap) = false;
#endif
    switch(A)
    {
    case MAXAR_0: longf(ap)= (LONG)MAXARITY; goto intret;
    case MAXDEP_0: longf(ap)= (LONG)MAXDEPTH; goto intret;
    case MAXINT_0: longf(ap)= maxlong; goto intret;
    case MININT_0: longf(ap)= minlong; goto intret;
    case PLUS_2: 
#if REALARITH
        if(isreal(at1)||isreal(at2))
        {
            realf(ap)= ris(at1) + ris(at2); 
            goto realret;
        }
#endif
        longf(ap)= lis(at1) + lis(at2); 
        goto intret;
    case MINUS_2: 
#if REALARITH
        if(isreal(at1)||isreal(at2))
        {
            realf(ap)= ris(at1) - ris(at2); 
            goto realret;
        }
#endif
        longf(ap)= lis(at1) - lis(at2); 
        goto intret;
    case TIMES_2: 
#if REALARITH
        if(isreal(at1)||isreal(at2))
        {
            realf(ap)= ris(at1) * ris(at2); 
            goto realret;
        }
#endif
        longf(ap)= lis(at1) * lis(at2); 
        goto intret;
    case DIVIDE_2: 
#if REALARITH
        if(ris(at2) == 0.0)
            ERROR(DIV0E);
        realf(ap)= ris(at1) / ris(at2); 
        goto realret;
#endif
    case IDIV_2: 
        if(lis(at2)==0)
               ERROR(DIV0E);
        longf(ap)= lis(at1) / lis(at2); 
        goto intret;
    case MINUS_1: 
#if REALARITH
        if(isreal(at1))
        {
            realf(ap)= -ris(at1); 
            goto realret;
        }
#endif
        longf(ap)= -lis(at1); 
        goto intret;
#if REALARITH
    case PI_0:       realf(ap)= PI_CONST; goto realret;
    case E_0:        realf(ap)= E_CONST; goto realret;
    case REAL_1:     realf(ap)= ris(at1); goto realret;
    case EXP_1:      realf(ap)= exp(ris(at1)); goto realret;
    case LN_1:       realf(ap)= log(ris(at1)); goto realret;
    case LOG10_1:    realf(ap)= log10(ris(at1)); goto realret;
    case SQRT_1:     realf(ap)= sqrt(ris(at1)); goto realret;
    case SIN_1:      realf(ap)= sin(ris(at1)); goto realret;
    case COS_1:      realf(ap)= cos(ris(at1)); goto realret;
    case TAN_1:      realf(ap)= tan(ris(at1)); goto realret;
    case ASIN_1:     realf(ap)= asin(ris(at1)); goto realret;
    case ACOS_1:     realf(ap)= acos(ris(at1)); goto realret;
    case ATAN_1:     realf(ap)= atan(ris(at1)); goto realret;
    case FLOOR_1:    realf(ap)= floor(ris(at1)); goto realret;
    case CEIL_1:     realf(ap)= ceil(ris(at1)); goto realret;
    case POWER_2:    realf(ap)= pow(ris(at1),ris(at2)); goto realret;
    case ENTIER_1:   longf(ap)= (LONG)floor(ris(at1)); goto intret;
#endif
    case LSHIFT_2:   longf(ap)= lis(at1) << lis(at2); goto intret;
    case RSHIFT_2:   longf(ap)= lis(at1) >> lis(at2); goto intret;
    case BITAND_2:   longf(ap)= lis(at1) & lis(at2); goto intret;
    case BITOR_2:    longf(ap)= lis(at1) | lis(at2); goto intret;
    case BITNEG_1:   longf(ap)= ~lis(at1); goto intret;
    case AND_2:      longf(ap)= lis(at1) && lis(at2); goto intret;
    case OR_2:       longf(ap)= lis(at1) || lis(at2); goto intret;
    case NEG_1:      longf(ap)= !lis(at1); goto intret;
    case MOD_2:  
        if(lis(at2)==0)
            ERROR(DIV0E);
        longf(ap)= lis(at1) % lis(at2); 
        goto intret;
    default:
        if(stdfunc)
            ERROR(UNDEFFUNCE);
        else
            return false;
        /*NOTREACHED*/
    }
#if REALARITH
realret: 
    isreal(ap) = true;
#endif
intret:  
    return true;
}

GLOBAL boolean DOREDUCE(B0,B1,eval)
    TERM B0,B1;
    boolean eval;
{ 
    ATOM A;
    ATP_TYPE atR,at1,at2;
#if REALARITH
    isreal(&at1)= isreal(&at2)= false;
#endif
    test("reduce: ",B1);

    A=name(B1);

    if(A==QUOTE_1)
    {
        B1= arg1(B1);
        goto reduced;
    }
    else if(arithbi(A))
    {
        switch(arity(A))
        {
            default: SYSTEMERROR("DOREDUCE.250");
            case 2: if(!assign(&at2,arg2(B1))) goto notreduce;
            case 1: if(!assign(&at1,arg1(B1))) goto notreduce;
            case 0: break;
        }
        if(compute(A,&at1,&at2,&atR,false))
        {
#if REALARITH
            if(isreal(&atR)) 
                return UNI(B0,mkreal(realf(&atR)));
#endif
            return LONGRES(B0,longf(&atR));
        }
    }
notreduce:
    if(eval)
        return false;
reduced:
    test("reduced to: ",B1);
    (void)UNI(B0,B1);
    return true;
}

LOCAL TERM new_calls;

LOCAL TERM putreduce(A,R)
    ATOM A;
    TERM R;
{
    TERM TT;
    TERM Result;
    TT = stackvar(arity(A));
    TT = mk2sons(UNBOUNDT,NIL_TERM,A,TT);
    Result = br(TT);
    (void)UNI(R,TT);
    TT = mk2sons(REDUCE_2,TT,GOTOT,new_calls);
    test("putreduce:",TT);
    new_calls = TT;
    return Result;
}

LOCAL void putcall(T,R)
    TERM T,R;
{
    TERM TT;
    ATOM A; 
    int ar;

    A = name(T);
    A = LOOKATOM(A,(ar=arity(A))+1,true);
    TT = mk2sons(UNBOUNDT,NIL_TERM,name(T),son(T));
    (void)UNI(TT,R);
    TT = mk2sons(A,stackvar(ar+1),ISEQ_2,TT);
    (void)UNI(son(TT),R);
    (void)UNIFY(ar,br(son(TT)),son(T),NIL_TERM,BE,MAXDEPTH);
    TT = mk2sons(SEMI_2,TT,GOTOT,new_calls);
    test("putcall:",TT);
    new_calls = TT;
}

LOCAL void eval(T,R,Depth)
    TERM T,R;
    int Depth;
{
    ATOM A;

    if(Depth == 0)
        ABORT(DEPTHE);

    if((A = name(T)) == QUOTE_1)
    {
        (void)UNI(arg1(T),R);
    }
#if ! STRINGS_AS_INT_LISTS
    else if(A == STRING_CONS || A == STRING_NIL)
    {
        (void)UNI(T,R);
    }
#endif
    else if(arithbi(A))
    {
        TERM TT;
        TT = putreduce(A,R);

        switch(arity(A))
        {
            default: SYSTEMERROR("DOREDUCE.250");
            case 2: eval(arg2(T),arg2(TT),Depth-1);
            case 1: eval(arg1(T),arg1(TT),Depth-1);
            case 0: break;
        }
    }
    else if(norm_atom(A))
    {
#if OPTIMIZE
        if(arity(A) == 0)
        {
            A = LOOKATOM(A,1,false);
            if(A != NIL_ATOM && clause(A) != NIL_CLAUSE && 
                     name(body(clause(A))) == NIL_ATOM && 
                     nvars(clause(A)) == 0)
            {
                (void)UNI(R,son(head(clause(A))));
                goto end;
            }
        }
#endif
        putcall(T,R);
    }
    else
        (void)UNI(T,R);

  end:;
      /* check0(); */
}
GLOBAL TERM DOEVAL(CP,CE)
    TERM CP;
    ENV CE;
{
#if OPTIMIZE
    TERM T,OLD_T;
#endif
    TERM TT;


    E = CE;
    BE = base(CE);
    new_calls = br(CP);
    eval(arg2(CP),arg1(CP),MAXDEPTH);
#if OPTIMIZE
    OLD_T = T = new_calls;
    while(T != br(CP))
    {
        test("NCP= ",T);
        if(name(T) == REDUCE_2 && DOREDUCE(arg1(T),arg2(T),true))
        {
            if(T == new_calls)
            {
                new_calls = son(br(T));
                T = OLD_T = new_calls;
                continue;
            }
            else
                son(br(OLD_T)) = son(br(T));
        }
        else
            OLD_T = T;
        T = son(br(T)); /* skip gotot */
    }
#endif
    TT =  mk2sons(name(CP),son(CP),GOTOT,new_calls);
    return TT;
}


LOCAL void iseval(t,ap,Depth) 
    TERM t;
    ATP_TYPE *ap;
    card Depth;
{
    ATP_TYPE at1,at2;
    TERM s;
    ATOM a;

    if(Depth == 0)
        ABORT(DEPTHE);
#if REALARITH
    isreal(ap)= isreal(&at1)= isreal(&at2)= false;
#endif
    if(!norm_atom(a=name(t))) 
        if(assign(ap,t)) 
            return;
        else 
            ERROR(STDFUNCARGE);
    test("evaluate: ",t);
    switch(arity(a))
    {
        default: ERROR(UNDEFFUNCE);
        case 2: s=arg2(t); if(!assign(&at2,s)) iseval(s,&at2,Depth-1);
        case 1: s=arg1(t); if(!assign(&at1,s)) iseval(s,&at1,Depth-1);
        case 0: break;
    }
    (void)compute(a,&at1,&at2,ap,true);
}

GLOBAL int INTVALUE(T)
    TERM T;
{ 
    ATP_TYPE a;
    deref(T);
    iseval(T,&a,MAXDEPTH);
    if(
#if REALARITH
    isreal(&a) || 
#endif
    (maxint < longf(&a)) || (minint > longf(&a)))
        ERROR(BADEXPE);
    return (int)longf(&a);
}

GLOBAL boolean DOACOMP()
{
#if REALARITH
    REAL R1,R2;
#endif
#if !REALARITH
    LONG R1,R2;
#endif
    TERM H1,H2;

    /* initialize R1 and R2 (for lint only) */
    R1 = R2 = 0;
    H1 = arg1(A0); 
    H2 = arg2(A0);
    switch(name(H1))
    {
#if REALARITH
    case REALT:   R1= realval(H1); break;
#if LONGARITH
    case LONGT:   R1= (REAL)(longval(H1)); break;
#endif
    case INTT:    R1= (REAL)((LONG)ival(H1)); break;
#endif
#if !REALARITH
#if LONGARITH
    case LONGT:   R1= longval(H1); break;
#endif
    case INTT:    R1= (LONG)ival(H1); break;
#endif
    default: ARGERROR();
    }
    switch(name(H2))
    {
#if REALARITH
    case REALT:   R2= realval(H2); break;
    case INTT:    R2= (REAL)((LONG)ival(H2)); break;
#if LONGARITH
    case LONGT:   R2= (REAL)(longval(H2)); break;
#endif
#endif
#if !REALARITH
    case INTT:    R2= (LONG)ival(H2); break;
#if LONGARITH
    case LONGT:   R2= longval(H2); break;
#endif
#endif
    default: ARGERROR();
    }
    switch(name(A0))
    {
        default: ARGERROR();
        case EQ_2: return R1 == R2;
        case NE_2: return R1 != R2;
        case GT_2: return R1 > R2;
        case LT_2: return R1 < R2;
        case GE_2: return R1 >= R2;
        case LE_2: return R1 <= R2;
    }
}

GLOBAL boolean DOIS() 
{ 
    ATP_TYPE a;
    iseval(A1,&a,MAXDEPTH);
#if REALARITH
    if(isreal(&a)) 
        return UNI(A0,mkreal(realf(&a)));
#endif
    return LONGRES(A0,longf(&a));
}

LOCAL boolean equal(HT , T , Depth)
    TERM HT; /* HEAPTERM whithout Stacksegment -- DON'T DEREF */
    TERM T;
    int Depth;
{
    deref(T);

    if(func_atom(name(T)) || func_atom(name(HT)))
    {
	int ar;
	if(name(T) != name(HT))
	    return false;
	for(ar = arity(name(T)) , HT = son(HT), T = son(T) ; 
	    ar > 0 ; 
	    T = br(T) , HT = br(HT) , --ar)
	{
	    if(!equal(HT, T, Depth-1))
		return false;
	}
    }
    else if(name(T) == name(HT) && name(T) == INTT)
	return ival(T) == ival(HT);
    return true;
}

GLOBAL boolean DODASS()
{
    ATOM A;
    CLAUSE CL;
    int ar;
    TERM T;
    boolean Result;


    if(!norm_atom(A = name(A0)))  
    {
        Result =  UNI(A0,A1);
	goto end;
    }


    ar = arity(A)+1;
    A = copyatom(LOOKATOM(A,ar,true));
    if((system(A) && !aSYSMODE) || class(A) != NORMP)
        ERROR(SYSPROCE);
    for (CL= clause(A);CL != NIL_CLAUSE;CL = nextcl(CL))
        if(name(body(CL)) == NIL_ATOM)
	{ 
	    T= son(head(CL));
	    if(arity(name(A0)) > 0)
	    {
		TERM TT;
		TT = mkfunc(name(A0), br(T));

		if(!equal(TT,A0,MAXDEPTH)) /* REIHENFOLGE */
		    continue;
	    }
	    if(func_atom(name(T)))
		freeterm(arity(name(T)),son(T),MAXDEPTH);
	    if((name(T)=copyatom(name(A1)))== INTT)
		ival(T)= ival(A1); 
	    else if(name(T) == UNBOUNDT)
	    {
		/* fehlerhafte Optimierung - geht nur bei ground 
		   Das Problem besteht darin, dass Aufrufe der Form

		   $dass(A,B), wo A UND B Variablen enthalten

		   IMMER falsch realisiert werden.
		*/
		if((WARNFLAG & WARN_ASSIGN) == WARN_ASSIGN && nvars(CL) && arity(name(A0)))
		{ 
		    START_WARNING();
		    ws("WARNING: :=/2 (resp. $dass/2) may be errornous\n");
		    END_WARNING();
		}

		name(T) = SKELT;
		offset(T) = 0;
		if(nvars(CL) == 0 || arity(name(A0)) == 0)
		    setnvars(CL,1);
	    }
	    else
	    {
		/* Fehlerhafte Optimierung - geht nur bei ground */
		Init_Skeleton();
		son(T)= SKELETON(son(A1),arity(name(A1)),MAXDEPTH);
		if(Var_Count() > nvars(CL))
		    setnvars(CL,Var_Count());
		if((WARNFLAG & WARN_ASSIGN) == WARN_ASSIGN && nvars(CL) && Var_Count())
		{ 
		    START_WARNING();
		    ws("WARNING: :=/2 (resp. $dass/2) may be errornous\n");
		    END_WARNING();
		}

	    }
	    test("neuer term: ",T);
	    Result = true;
	    goto end;
	}
    T= stackvar(ar); 
    (void)UNI(T,A1);
    (void)UNIFY(ar-1,br(T),son(A0),NIL_TERM,BE,MAXDEPTH);
    T= mkfunc(A,T);
    test("vor assert: ",T);
    CL=NewClause(T,NIL_TERM,false);
    test("nach assert: ",head(CL));
    nextcl(CL)=clause(A); 
    clause(A)=CL;
    set_in_use(CL);
    Result = true;
  end:;
    return Result;
}

LOCAL ATOM build_in_tab[] = {

    PLUS_2, MINUS_2, TIMES_2, LSHIFT_2, RSHIFT_2, BITAND_2, 
    BITOR_2, AND_2, OR_2, IDIV_2, MOD_2,
    MINUS_1, BITNEG_1, NEG_1,
    MAXINT_0, MININT_0, MAXAR_0, MAXDEP_0,
#if REALARITH
    DIVIDE_2, POWER_2,
    FLOOR_1, CEIL_1, ENTIER_1, EXP_1, LN_1, LOG10_1, SQRT_1, 
    SIN_1, COS_1, TAN_1, ASIN_1, ACOS_1, ATAN_1, REAL_1,
    E_0, PI_0,
#endif

    NIL_ATOM
};

GLOBAL void Init_Arith()
{
    ATOM *AP;
#if DEBUG
    if(DEBUGFLAG)
	out(2,"Init_Arith()\n");
#endif
    for(AP = build_in_tab;*AP != NIL_ATOM;++AP)
        setarithbi(*AP);
}


/* end of file */
