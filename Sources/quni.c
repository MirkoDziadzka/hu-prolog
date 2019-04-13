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

#ifndef INLINE
#define INLINE 1
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
    register TERM T;
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

#define TC      termtop(CHOICEPOINT)
#if !NO_VOID_CAST
#define trailvar(V) (is_older_term(TC,V) ? (void)0 :(void)trailterm(V))
#endif
#if NO_VOID_CAST
#define trailvar(V) (is_older_term(TC,V) ? 0 : (trailterm(V),0))
#endif

#define bind(V,T,A,B) (trailvar(V),(name(V)=A),(son(V) = \
    (is_heap_term(T) ? copy_block(son(T),arity(A),B,Depth):son(T))))

#define bind_int(V,I)  ((name(V)=INTT),(ival(V)=ival(I)),trailvar(V))
#define bind_vars(Y,O) ((name(Y)=VART),(val(Y)=O),trailvar(Y))

LOCAL TERM copy_block(T,ar,BB,Depth)
    TERM T;
    TERM BB;
    register int ar;
    int Depth;
{
    TERM Result;
    register TERM Tmp;

    if(Depth == 0)
        ABORT(DEPTHE);
#if !INLINE
    Result = Tmp = stackterm(ar); 
#endif
#if INLINE
    if(ar == 0)
        return NIL_TERM;
    if((GLOTOP -= ar*TERM_UNIT) <= HEAPTOP) 
        ABORT(LOCALSPACEE);
    Result = Tmp = GLOTOP;
#endif
    while(--ar >= 0)
    {
        register TERM TT;
        register ATOM A;
        TT = T;
#if !INLINE
        deref_(TT,BB);
#endif
#if INLINE
        if(name(TT) == SKELT)
        {
            TT = n_brother(BB,offset(TT));
            while(name(TT) == VART)
                TT = val(TT);
            /* TT steht jetzt im Stack !! */
            if((name(Tmp) = name(TT)) == UNBOUNDT)
            {
                /* is_older_term(TT,Tmp) */
                bind_vars(Tmp,TT);  
            }
            else
                son(Tmp) = son(TT); /* int or son */
        }
        else
#endif
        if((A = name(Tmp) = name(TT)) == UNBOUNDT)
        {
            /* is_older_term(TT,Tmp) */
            bind_vars(Tmp,TT);
        }
        else if(A == INTT)
            ival(Tmp) = ival(TT);
        else
            son(Tmp) = copy_block(son(TT),arity(A),BB,Depth-1);
        next_br(T);
        next_br(Tmp);
    }
    return Result;
}

#if OCCUR_CHECK
#define make_ocheck(V,T,B) if(OCHECK && Occur(V,T,B,Depth)) goto fail
#endif
#if !OCCUR_CHECK
#define make_ocheck(V,T,B)  /* nothing */
#endif

#if HIGHER_ORDER
LOCAL TERM normalize_var_call(T,B)
    TERM T,B;
{
    return T;
}

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
    else /* b1 == b2 (both are var_call's */
	goto fail;

    return true;
fail:;
    return false;
}
#endif


GLOBAL boolean UNIFY(N,Term2,Term1,B2,B1,Depth)
    TERM Term1,Term2;
    TERM B1,B2;
    int N;
    int Depth;
{
    TRAIL T_TOP;
    TERM  S_TOP;
    register TERM T1,T2;

    S_TOP = mark_term_stack();
    T_TOP = mark_trail_stack();

    if(Depth <= 0)
	ABORT(DEPTHE);
    while(N-- != 0)
    {
        register ATOM A;
        T1 = Term1; 
        T2 = Term2;
        deref_(T2,B2);
        if(!func_atom(A=name(T1)))
        {
            deref_(T1,B1);
            if((A = name(T1)) == UNBOUNDT)
            {
                if(func_atom(A = name(T2)))
                {
                    make_ocheck(T1,T2,B2);
                    bind(T1,T2,A,B2);
                }
                else if(A == INTT)
                    bind_int(T1,T2);
                else /* if(A  == UNBOUNDT) */
                {
                    if(is_older_term(T1,T2))
                        bind_vars(T2,T1);
                    else if(is_older_term(T2,T1))
                        bind_vars(T1,T2);
                    else /* T1 == T2 */
                        ;
                }
                next_br(Term1); next_br(Term2); continue;
            }
            else if(A == INTT)
            {
                if((A = name(T2)) == UNBOUNDT)
                    bind_int(T2,T1);
                else if(A != INTT || ival(T1) != ival(T2))
                    goto fail;
                next_br(Term1); next_br(Term2); continue;
            }
            else /* funcname(A = name(T1)) */
                goto func1;
        }
    func1: /* func_atom(A = name(T1)) */
        if((name(T2)) == UNBOUNDT)
        {
            make_ocheck(T2,T1,B1);
            bind(T2,T1,A,B1);
            next_br(Term1); next_br(Term2); continue;
        }
    /* func2:*/ /*  A = name(T1) */
        if(name(T2) != A)
	    goto fail;
	else if(!UNIFY(arity(A),son(T1),son(T2), B1,B2,Depth-1))
            goto fail;

        next_br(Term1); next_br(Term2);continue;
    }
    return true;

fail:;
#if HIGHER_ORDER
    if(var_call(name(T1)) || var_call(name(T2)))
    {
	T1 = normalize_var_call(T1,B1);
	T2 = normalize_var_call(T2,B2);
	if(var_call_unify(T1,T2,B1,B2,Depth-1))
	    goto next;
    }
#endif
    rel_term_stack(S_TOP);
    rel_trail_stack(T_TOP);
    return false;
}


/* end of file */
