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
#include "extern.h"

#ifndef DEREF_
GLOBAL TERM DEREF_(T,B)
    TERM T,B;
{
    if(name(T) == SKELT)
        T = n_brother(B,offset(T));
    while(name(T) == VART)
        T = val(T);
    return T;
}
#endif

#ifndef INTRES
GLOBAL boolean INTRES (X, I)
    TERM X;
    int I;
{
    return UNI(X,mkint(I));
}
#endif

#if LONGARITH
GLOBAL boolean LONGRES(T,L)
    TERM T;
    LONG L;
{
    if(minint<=L && L<=maxint)
        return INTRES(T,(int)L);
    else 
        return UNI(T,mklong(L));
}
#endif

#if OCCUR_CHECK
LOCAL boolean Occur(V,T,B,Depth)
    TERM V;
    TERM T;
    TERM B;
    int Depth;
{
    int ar;
    if(Depth == 0) ABORT(DEPTHE);
    deref_(T,B);
    if(T == V) return true;
    ar = arity(name(T));
    T = son(T);
    while(--ar >= 0)
    {
        if(Occur(V,T,B,Depth-1)) return true;
        next_br(T);
    }
    return false;
}
#endif

#if QUICK_BUT_DIRTY
#define trailvar(V) if(!is_older_term(termtop(CHOICEPOINT),V))\
        trailterm(V);else
#endif
#if !QUICK_BUT_DIRTY
LOCAL void trailvar(V)
    TERM V;
{
#if DEBUG
    if(CHOICEPOINT == NIL_ENV)
        SYSTEMERROR("trailvar - bad arg");
#endif
    if(!is_older_term(termtop(CHOICEPOINT),V))
        trailterm(V);
}
#endif

LOCAL TERM copy_block(T,ar,B,Depth)
    TERM T;
    TERM B;
    int ar;
    int Depth;
{
    TERM Result;
    TERM Tmp;

    if(Depth == 0)
        ABORT(DEPTHE);
    Result = Tmp = stackterm(ar);
    while(ar-- > 0)
    {
        TERM TT;
        TT = T;
        deref_(TT,B);
        name(Tmp) = name(TT);
        if(name(TT) == INTT)
            ival(Tmp) = ival(TT);
        else if(name(TT) == UNBOUNDT)
        {
            /* BIND(TT,Tmp,B,1); */
            name(Tmp) = VART;
            val(Tmp)  = TT;
            trailvar(Tmp);
        }
        else
            son(Tmp) = copy_block(son(TT),arity(name(TT)),B,Depth-1);
        next_br(T);
        next_br(Tmp);
    }
    return Result;
}

LOCAL void BIND(V,T,B,Depth)
    register TERM V,T;
    TERM B;
    int Depth;
{
    if(Depth == 0) ABORT(DEPTHE);

    if(name(T) == UNBOUNDT)
    {
        if(T == V) return;
        if(is_older_term(V,T))
        {
            name(T) = VART; 
            val(T) = V;
            trailvar(T);
        }
        else /* is_older_term(T,V) */
        {
            name(V) = VART; 
            val(V)  = T;
            trailvar(V);
        }
    }
    else if(name(T) == INTT)
    {
        name(V) = INTT;
        ival(V) = ival(T);
        trailvar(V);
    }
    else
    {
        trailvar(V);
        name(V) = name(T);
        if(is_heap_term(T))
            son(V) = copy_block(son(T),arity(name(T)),B,Depth-1);
        else
            son(V)  = son(T);
    }
}

#if HIGHER_ORDER
LOCAL boolean var_call_unify(T1,T2,B1,B2,Depth)
    TERM T1,T2;
    TERM B1,B2;
    int Depth;

{
    boolean b1,b2;
    TERM T;

    b1 = var_call(name(T1));
    b2 = var_call(name(T2));

    if(b1 != b2)
    {
	if(b1)
	{
	    if((arity(name(T2)) + 1) != arity(name(T1)))
		goto fail;
	    if(!UNIFY(arity(name(T2)),br(son(T1)),son(T2),B1,B2,Depth-1))
		goto fail;
	    T = mkatom(LOOKATOM(name(T2), 0, true));
	    if(!UNIFY(1, son(T1) , T, B1, NIL_ENV, Depth-1))
		goto fail;
	}
	else /* b2 */
	{
	    if((arity(name(T1)) + 1) != arity(name(T2)))
		goto fail;
	    if(!UNIFY(arity(name(T1)),son(T1),br(son(T2)),B1,B2,Depth-1))
		goto fail;
	    T = mkatom(LOOKATOM(name(T1), 0, true));
	    if(!UNIFY(1, T , son(T2), NIL_ENV, B2, Depth-1))
		goto fail;
	}
    }
    else
	goto fail;

    return true;
fail:;
    return false;

}
#endif

GLOBAL boolean UNIFY(N,Term1,Term2,B1,B2,Depth)
    TERM Term1,Term2;
    register TERM B1,B2;
    int N;
    int Depth;
{
    register TERM T1,T2;
    TRAIL T_TOP;
    TERM  S_TOP;

    if(Depth == 0) ABORT(DEPTHE);

    S_TOP = mark_term_stack();
    T_TOP = mark_trail_stack();

    while(N-- > 0)
    {
        T1 = Term1; 
        T2 = Term2;
        deref_(T1,B1);
        deref_(T2,B2);
        if( T1 == T2 )
            /* ok */ ;
        else if(name(T1) == UNBOUNDT)
        {
#if OCCUR_CHECK
            if(OCHECK && Occur(T1,T2,B2,Depth-1)) 
                goto fail;
#endif
            BIND(T1,T2,B2,Depth-1);
        }
        else if(name(T2) == UNBOUNDT)
        {
#if OCCUR_CHECK
            if(OCHECK && Occur(T2,T1,B1,Depth-1)) 
                goto fail;
#endif
            BIND(T2,T1,B1,Depth-1);
        }
        else if(name(T1) != name(T2)) 
	    goto fail;
        else if(name(T1) == INTT)
        {
            if(ival(T1) != ival(T2))
                goto fail;
        }
        else if(!UNIFY(arity(name(T1)),son(T1),son(T2),B1,B2,Depth-1))
            goto fail;

    next:;
        next_br(Term1);
        next_br(Term2);
    }
    return true;
fail:
#if HIGHER_ORDER
    if(var_call(name(T1)) || var_call(name(T2)))
    {
	if(var_call_unify(T1,T2,B1,B2,Depth-1))
	    goto next;
    }
#endif
    rel_term_stack(S_TOP);
    rel_trail_stack(T_TOP);
    return false;
}


#ifndef UNI
GLOBAL boolean UNI(T1,T2)
    TERM T1,T2;
{
    return UNIFY(1,T1,T2,BE,BE,MAXDEPTH);
}
#endif


/* end of file */
