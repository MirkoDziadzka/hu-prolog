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
#include "extern.h"



/****************************************************************
**   Implementation der Atomtabelle                            **
****************************************************************/


LOCAL int idhash(A)
register ATOM A;
{
    STRING index;
    index=atomstring(A);
    return hashcode(stringchar(index,0),stringchar(index,1));
}

LOCAL char s_tab[ MAX_NAME_LEN+1 ];
LOCAL int NEWINDEX;
GLOBAL string NEWATOM = s_tab;

GLOBAL void STARTATOM ()
{
    NEWINDEX = 0;
    NEWATOM = s_tab;
}

GLOBAL void ATOMCHAR (C)
    char C;
{
    if (NEWINDEX >= MAX_NAME_LEN)
    {
        ERROR(aSTRINGSPACEE);
    }
    s_tab[NEWINDEX++] = C;
}


LOCAL int idstrcmp(A,str)
    ATOM A;
    string str;
{
    STRING S;
    int index = 0;
    S=atomstring(A);
    while(*str == stringchar(S,index))
    {
        if(*str++) 
            ++index;
        else 
            return 0;
    }
    return (stringchar(S,index) - *str);
}

LOCAL ATOM CONSTATOM;  
LOCAL boolean INIT;

LOCAL void initfields(A,ar)
    ATOM A;
    int ar;
{
    bit1(A)=0;
    bit2(A)=0;
    setoprec(A,0);
    clause(A)= NIL_CLAUSE;
    arity(A) = ar;
    nextatom(A) = NIL_ATOM;
    chainatom(A) = A;
    setnotlib(A);
}

GLOBAL ATOM LOOKUP (str,ar,heap,create)
    string str;
    int ar;
    boolean heap; 
    boolean create;
{
    register ATOM A,OA;
    ATOM NA,HASHATOM;
    int cmp,H,nf;

    OA=NA = NIL_ATOM;
    nf = 0;
    cmp = 0; /* for lint only */
    H=strhash(str);
    if((ar < 0) || (ar > MAXARITY)) 
        ERROR(BADARITYE);
    HASHATOM = HASHTAB[ H ];
    if(HASHATOM != NIL_ATOM) /* search in primary chain */
    {
        OA = A = HASHATOM;
        while(A != NIL_ATOM && (cmp = idstrcmp(A,str)) < 0)
        {
            OA=A;
            A=nextatom(A);
            ++nf;
        }
        if(A != NIL_ATOM  && cmp == 0) 
            NA = A;
        while(A != NIL_ATOM && (cmp = idstrcmp(A,str)) == 0 && 
            (hide(A) || private(A)))
        {
            OA=A;
            A=nextatom(A);
            nf++;
        }
        if(A == NIL_ATOM) 
            cmp = 1;
        if(A != NIL_ATOM && cmp == 0) /* search in secondary chain */
        {
            int AA,OAA;
            nf++;
            NA = A;
            AA=arity(A);
            if(ar == AA) 
                goto found;
            do {
                OA = A; 
                OAA = AA;
                A = chainatom(A); 
                AA = arity(A);
                if(ar == AA) 
                    goto found;
            } while(! ((ar > OAA && ar < AA) || 
                (OAA >= AA && (ar > OAA || ar < AA))));
        }
    }
    if(!heap)  /* search atom in stack */
    {
        A = NIL_ATOM;
        while((A = get_stack_atom(A)) != NIL_ATOM)
            if(idstrcmp(A,str) == 0)
            { 
                NA=A; 
                if(ar==arity(A))
                    goto found; 
            }
    }
    if(create) /* create atom */
    {
        if(INIT) 
            A= CONSTATOM;
        else if(heap) 
            A= heapatom();
        else 
            A= stackatom(); 

        if(NA != NIL_ATOM ) 
            atomstring(A)= atomstring(NA);
        else if(heap) 
            atomstring(A)= heapstring(str);
        else 
            atomstring(A)= stackstring(str);
        initfields(A,ar);
        setfirst(A);
        if(heap)
        {
            if(HASHATOM == NIL_ATOM || nf == 0 )
            {
                nextatom(A) = HASHTAB[H];
                HASHTAB[H] = A;
                goto created;
            }
            if(cmp != 0)
            {
                nextatom(A) = nextatom(OA);
                nextatom(OA) = A;
            }
            else
            {
                setnotfirst(A);
                chainatom(A) = chainatom(OA);
                nextatom(A) = NIL_ATOM;
                chainatom(OA) = A;
            }
        }
    goto created;
    }
    else 
        A = NIL_ATOM;
found:
    /* in der Initialisierungphase muss alles neu angelegt werden */
    if(INIT)  
        SYSTEMERROR("LOOKUP");
created:
    STARTATOM();
    return A;
}

#if 0
#ifndef var_call
GLOBAL boolean var_call(A)
    register ATOM A;
{
    register int ar;
    if(is_heap_atom(A) && hide(A))
    {
	ar = arity(A);
	if(ar != 0)
	    do
		A = chainatom(A);
	    while(arity(A) > ar);
	return A == VAR_CALL;
    }
    return false;
}
#endif
#endif

GLOBAL ATOM LOOKATOM(A,ar,create)
    ATOM A;
    int ar;
    boolean create;
{
    register ATOM OA;
    ATOM ORG_A;
    boolean heap=false;
    if(ar < 0 || ar > MAXARITY) 
        ERROR(BADARITYE);
    ORG_A = A;
    if(is_heap_atom(A))
    {
        register int AA,OAA;
        if(( AA = arity(A)) == ar) 
            return A;
        if(private(A) || hide(A)) 
            heap = true;

        do {
            OA = A; 
            OAA = AA;
            A = chainatom(A); 
            AA = arity(A);
            if(ar == AA) 
                return A;
        } while(! ((ar > OAA && ar < AA) || 
            (OAA >= AA && (ar > OAA || ar < AA))));
        /* not found */
        if(heap)
	{
            if(create)
            {
                A= heapatom();
                atomstring(A)= atomstring(OA);
                initfields(A,ar);
                chainatom(A) = chainatom(OA);
                chainatom(OA) = A;
                nextatom(A) =  NIL_ATOM;
                if(private(OA))
                    setprivate(A);
                if(hide(OA))
                    sethide(A);
#if HIGHER_ORDER
		if(var_call(ORG_A))
		    setclass(A,VARCP);
#endif
                return A;
            }
            else 
	    {
                return NIL_ATOM;
	    }
	}
    }
    return LOOKUP(tempstring(atomstring(ORG_A)),ar,heap,create);
}

GLOBAL ATOM nameof(X)
    TERM X;
{ 
    deref(X); 
    return name(X); 
}

GLOBAL ATOM atom(X)   
    TERM X;
{
    ATOM A;
    if(name(X)!=DIVIDE_2) 
        ERROR(ARGE);
    A = LOOKATOM(nameof(arg1(X)),INTVALUE(arg2(X)),true);
    if(A == NIL_ATOM)
        ERROR(ARGE);
    return A;
}

GLOBAL ATOM copyatom(A)
    ATOM A;
{
    ATOM NA;
    TERM T;

    if(is_heap_atom(A))
        return(A);
    NA=LOOKUP(tempstring(atomstring(A)),(int)arity(A),true,true);
    T = NIL_TERM;
    while((T = get_stack_term(T)) != NIL_TERM)
        if(name(T)==A)
            name(T)=NA;
    setrc(NA); /* for reconsult */
    return NA;
}


LOCAL void PRIVATE(A)
    ATOM A;
{
    register ATOM AA;
    AA = A = copyatom(A);
    do {
        setprivate(A); 
        A = chainatom(A);
    } while(A != AA);
}

LOCAL void HIDE(A)
    ATOM A;
{
    string str;
    int cmp;
    ATOM AA ;

    AA = A = copyatom(A);
    str = tempstring(atomstring(A));
    do { 
        sethide(A); 
        A = chainatom(A); 
    } while(A != AA);
    A = HASHTAB[strhash(str)];
    while(A != NIL_ATOM  && (cmp = idstrcmp(A,str)) <= 0 )
    {
        if(cmp == 0 && !hide(A) && private(A))
            AA = A;
        A = nextatom(A);
    }
    A = AA;
    do { 
        setnotprivate(A); 
        A = chainatom(A); 
    } while(A != AA);
}

GLOBAL void DOPRIVATE()
{ 
    while(name(A0) == CONS_2)
    { 
        PRIVATE(nameof(son(A0))); 
        A0=arg2(A0); 
    }
    if(name(A0) != NIL_0) 
        PRIVATE(name(A0));
}

GLOBAL void DOHIDE()
{
    while(name(A0) == CONS_2)
    { 
        HIDE(nameof(son(A0))); 
        A0=arg2(A0); 
    }
    if(name(A0) != NIL_0) 
        HIDE(name(A0));
}

/* A Prolog list of the characters of s: cf. 'atom'. */
GLOBAL TERM LISTREP (s,cons,nil)
    string s;
    ATOM cons;
    ATOM nil;
{ 
    register TERM  X;
    register int  len;
    len = strlen(s);
    if(len-- == 0) 
        return mkatom(nil);
    X = mk2sons(INTT,(int)(unsigned char)(char)s[len],nil,NIL_TERM);
    while(--len >= 0)
        X = mk2sons(INTT,(int)(unsigned char)(char)s[len],cons,X);
    return mkfunc(cons,X);
}

GLOBAL ATOM GetAtom(A)
    ATOM A;
{
    register int count;
start:;
    if(A == NIL_ATOM) 
        count = 0;
    else 
    {
        A = chainatom(A);
        if(!first(A)) 
            goto found;
        if(nextatom(A)) 
        { 
            A = nextatom(A);
            goto found;
        }
        count = idhash(A)+1;
    }
    while(count < HASHSIZE && HASHTAB[count] == NIL_ATOM) 
        ++count;
    if(count < HASHSIZE) 
        A= HASHTAB[count];
    else 
        A= NIL_ATOM;
found:;
    if(A != NIL_ATOM && ( private(A) || hide(A) )) 
        goto start;
    return A;
}


GLOBAL boolean DONAME ()
{
    if(norm_atom(name(A0)))
        return UNI(A1,LISTREP(tempstring(atomstring(name(A0))),CONS_2,NIL_0));
    else if(name(A0) == UNBOUNDT)
    {
        int C;
        STARTATOM();
        for( ; name(A1) == CONS_2 ; A1 = arg2(A1))
        {
            C= INTVALUE(arg1(A1));
            if (C <= 0 || C > 255) 
                ERROR(ARGE);
            ATOMCHAR((char)(unsigned char)C);
        }
        if(name(A1) != NIL_0)
            ERROR( ARGE );
        ATOMCHAR('\0');
        return UNI(A0,mkatom(LOOKUP(NEWATOM,0,false,true)));
    }
    else if(name(A0) == INTT)
        return UNI(A1,LISTREP(itoa(ival(A0)),CONS_2,NIL_0));
#if LONGARITH
    else if(name(A0) == LONGT)
        return UNI(A1,LISTREP(ltoa(longval(A0)),CONS_2,NIL_0));
#endif
#if REALARITH
    else if(name(A0) == REALT)
        return UNI(A1,LISTREP(ftoa(realval(A0)),CONS_2,NIL_0));
#endif
    else
        SYSTEMERROR("DONAME");
    /*NOTREACHED*/
    return false ;
}

GLOBAL void DOOP()
{
    int  prec,ar;
    ATOM  A;
    OCLASS F;
    TERM T;

    if ((prec=INTVALUE(A0)) < 0 || prec > MAXPREC) 
        ERROR(ARGE);

    ar = 0 ; F = NONO; /* for lint only */
    switch(A= name(A1))
    {
        case FX_0:   F=FXO ; ar=1; break;
        case FY_0:   F=FYO;  ar=1; break;
        case XF_0:   F=XFO;  ar=1; break;
        case YF_0:   F=YFO;  ar=1; break;
        case XFX_0:  F=XFXO; ar=2; break;
        case XFY_0:  F=XFYO; ar=2; break;
        case YFX_0:  F=YFXO; ar=2; break;
        default:     ERROR(ARGE);
    }
    if(prec == 0) 
        F = NONO;
    do
    {
        if(name(A2) == CONS_2)
        {
            T = son(A2); 
            A2=arg2(A2);
            if(name(A2) == NIL_0) 
                A2 = NIL_TERM;
        }
        else 
        {
            T= A2; 
            A2= NIL_TERM;
        }
        deref(T);
        CHECKATOM(T);
        A= copyatom( LOOKATOM(name(T),ar,true) );
        if(system(A) && !aSYSMODE) 
            ERROR(SYSPROCE);

        if((WARNFLAG & WARN_OP) == WARN_OP && prec != 0)
        {
            ATOM AA;
            OCLASS F1,F2;

            AA = LOOKATOM(A,1,false); 
            F1 = (AA != NIL_ATOM ? oclass(AA) : NONO);
            AA = LOOKATOM(A,2,false); 
            F2 = (AA != NIL_ATOM ? oclass(AA) : NONO);

            if (oclass(A) != NONO)
            { 
		START_WARNING();
                ws("WARNING: redeclaration of operator ");
                wq(A); ws("/"); ws(itoa(ar)); ws("\n");
		END_WARNING();
            }
            if( /* infix-postfix-conflict */
            ( (ar==2) &&(F1==FXO || F1==FYO)) ||
                ((F==XFO || F==YFO)&&(F2==XFXO || F2==XFYO || F2==YFXO)))
            { 
		START_WARNING();
                ws("WARNING: conflicting infix/postfix ");
                ws("declaration for "); wq(A); ws("\n");
		END_WARNING();
            }
        }
        setoclass(A,(int)F); 
        setoprec(A,prec);
    } while (A2 != NIL_TERM);
}



/************ I N I T I A L I S A T I O N ***************/


#define sysflag 0x4000

LOCAL struct { 
    int  macro;
    string str;
    char predtype;
    char optype;
    int  prec;
}

InitT[] = 
{
#include "atom.tab"
{ NIL_ATOM    , "\0"        , 0       , 0    , 0           } 
};


GLOBAL void Init_Atoms()
{ 
    register int I;
    int Arity,Oprec,Predtype,Optype;
    string Name;
    ATOM A;

#if DEBUG
    if(DEBUGFLAG)
        out(2,"Init_Atoms\n");
#endif
    for(I=0;I<HASHSIZE;++I) 
        HASHTAB[I]=NIL_ATOM;
    INIT= true;
    for (I=0;InitT[I].macro != NIL_ATOM ;I++)
    { 
        CONSTATOM=InitT[I].macro;
        Name=InitT[I].str;
        Optype=InitT[I].optype;
        Predtype=InitT[I].predtype;
        Oprec=InitT[I].prec & ~sysflag;
        switch(Optype)
        { 
        case XFXO : 
        case XFYO : 
        case YFXO : Arity=2; break;
        case NONO : Arity=Oprec; Oprec=0; break;
        default   : Arity=1; break;
        }
        A=LOOKUP(Name,Arity,true,true);
        setoprec(A,Oprec);
        if (InitT[I].prec & sysflag) 
            setsystem(A);
        setoclass(A,Optype); 
        setclass(A,Predtype);
    }
    INIT = false;

    setclass(SKELT,VARP);
    setclass(VART,VARP);
    setclass(UNBOUNDT,VARP);
    setclass(GOTOT,GOTOP);
    setclass(INTT,NUMBP);
#if LONGARITH
    setclass(LONGT,NUMBP);
#endif
#if REALARITH
    setclass(REALT,NUMBP);
#endif

    setsystem(SKELT);
    setsystem(VART);
    setsystem(UNBOUNDT);
    setsystem(GOTOT);
    setsystem(INTT);
#if LONGARITH
    setsystem(LONGT);
#endif
#if REALARITH
    setsystem(REALT);
#endif
    sethide(GC_MARKER);
#if HIGHER_ORDER
    sethide(VAR_CALL);
    setclass(VAR_CALL,VARCP);
#endif
}

#if USER
GLOBAL void InitUAtom(A,s,oc,ar,cl,sys)
    ATOM A;
    string s;
    OCLASS oc;
    int ar;
    CLASS cl;
    boolean sys;
{
    int Arity;
    INIT= true;
    CONSTATOM = A;
    reserve_atom(A);
    switch(oc)
    { 
        case XFXO : 
        case XFYO : 
        case YFXO : Arity=2; break;
        case NONO : Arity=ar;ar = 0;break;
        default   : Arity=1; break;
    }
    (void)LOOKUP(s,Arity,true,true); 
    setoprec(A,ar);
    if(sys)
        setsystem(A);
    setoclass(A,oc); 
    setclass(A,cl);
    INIT = false;
}
#endif


/* end of file */
