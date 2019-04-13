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

GLOBAL void freeterm(N,T,Depth)
    int N;
    TERM T;
    int Depth;
{
    register TERM TT;
    register int NN;

    if(Depth == 0)
        ABORT(DEPTHE);

    for(TT=T,NN=N; NN != 0; next_br(TT), --NN)
        if(func_atom(name(TT)))
            freeterm(arity(name(TT)),son(TT),Depth-1);
    freeblock(N,T);
}

LOCAL TERM VAR_TAB[ MAXVARS ];

LOCAL int VARCT;

GLOBAL void Init_Skeleton()
{
    VARCT = 0;
}

GLOBAL int Var_Count()
{
    return VARCT;
}

GLOBAL TERM SKELETON (T,N,Depth)
    TERM T;
    int N;
    int Depth;
{ 
    TERM Result,S;
    TERM TT;
    ATOM A;

    if (Depth == 0)  
        ABORT(DEPTHE);

    Result = S = heapterm(N);
    for (;N != 0;next_br(S),next_br(T),--N)
    { 
        TT = T;
        deref(TT);
        if ((A=name(TT)) == UNBOUNDT)
        { 
            int J;

            for(J=0 ; J < VARCT ; ++J) 
                if(VAR_TAB[J]==TT) 
                    goto heapvar;
            J = VARCT++;
            if(VARCT>=MAXVARS) 
                ABORT(NVARSE);
            VAR_TAB[J]= TT;
heapvar: 
            name(S)=SKELT; 
            offset(S)=J;
        }
        else if (A == INTT) 
        { 
            name(S)=INTT; 
            ival(S)=ival(TT); 
        }
        else 
        { 
            name(S)= copyatom(A);
            son(S)=SKELETON(son(TT),arity(A),Depth-1);
        }
    }
    return Result;
}

GLOBAL CLAUSE NewClause(Head,Body,user_unify)
    TERM Head,Body;
    boolean user_unify;
{ 
    ATOM A;
    CLAUSE CL;

    Init_Skeleton();
    deref(Head);
    A=copyatom(name(Head));
#if USER_UNIFY
    int ar;
    if( user_unify && USER_UNIFY_FLAG && ( A != UNIFY_2 ) && (ar = arity(A)) )
    {
        TERM NewHead, T ;
	int i;

	NewHead = mkfunc(A , stackvar(ar));
	for( i = 1 ; i <= ar ; ++i )
	{
	    T = stackterm(2);
	    if( Body == NIL_TERM )
		Body = mkfunc(UNIFY_2 , T );
	    else
		Body  = mkfunc(COMMA_2, 
			       mk2sons(UNIFY_2 ,T , VART , Body));
	    name(T) = VART;
	    val(T)  = arg(i, NewHead);
	    next_br(T);
	    name(T) = VART;
	    val(T)  = arg(i , Head );
	}
	Head = NewHead;
    }
#endif
    Head = SKELETON(son(Head),arity(A),MAXDEPTH);
    if (Body == NIL_TERM)
    {
        CL=heapterm(_head_offset+2);
        name(body(CL))=NIL_ATOM; 
        son(body(CL))=NIL_TERM;
    }
    else /* Body != NIL_TERM */
    { 
        int I;
        TERM X,Z,ZZ;

        deref(Body);
        Z=Body;
        I=_head_offset + 1 + 2;
        while (name(Z)==COMMA_2) 
        { 
            Z=arg2(Z); 
            I++; 
        }
        if (I>=MAXARITY)  /* hier sollte stattdessen ein  */
            ABORT(DEPTHE);/* neues Segment angefordert werden */
        CL=heapterm(I);
        ZZ=body(CL); 
        Z=Body;
skel_1:
        if(name(Z) == COMMA_2) 
            X=arg1(Z); 
        else 
            X = Z;
        if (name(X)==UNBOUNDT)
        {
            register int J;
            for(J=0;J < VARCT ; J++)
                if(VAR_TAB[J]==X) goto heapvar;
            J = VARCT++;
            if(VARCT>=MAXVARS) ABORT(NVARSE);
            VAR_TAB[J]= X;
heapvar: 
            name(ZZ)=SKELT; 
            offset(ZZ)=J;
        }
        else if(name(X) == INTT)
        {
            name(ZZ) = INTT;
            ival(ZZ) = ival(X);
        }
        else
        {
            name(ZZ)=copyatom(name(X));
            son(ZZ)=SKELETON(son(X),arity(name(ZZ)),MAXDEPTH);
        }
        next_br(ZZ);
        if(name(Z) == COMMA_2)
        {
            Z = arg2(Z); 
            goto skel_1;
        }
        name(ZZ)=NIL_ATOM; 
        son(ZZ)=NIL_TERM;
    }
    if (aSYSMODE) 
        setsystem(A);
    nextcl(CL)=NIL_CLAUSE; 
    setnvars(CL,VARCT);
    name(head(CL))=A; 
    son(head(CL))=Head;
    return CL;
}

GLOBAL TERM ClauseHead(CL,B)
    CLAUSE CL;
    TERM B;
{
    TERM H;
    H=mkfreevar();
    (void)UNIFY(1,H,head(CL),NIL_TERM,B,MAXDEPTH);
    return H;
}

GLOBAL TERM ClauseBody(CL,B)
    CLAUSE CL;
    TERM B;
{
    TERM T,TT;
    TERM Result;
    if(name(T = body(CL)) == NIL_ATOM)
        return NIL_TERM;
    while(name(T) == GOTOT)
       T = son(T);
    if(name(br(T)) == NIL_ATOM)
    {
        Result = mkfreevar();
        (void)UNIFY(1,Result,T,NIL_TERM,B,MAXDEPTH);
        return Result;
    }
    Result = TT = mkfunc(COMMA_2,stackvar(2));
    for(;;)
    {
        (void)UNIFY(1,T,son(TT),B,NIL_TERM,MAXDEPTH);
        TT = br(son(TT));
        next_br(T);
        while(name(T) == GOTOT)
           T = son(T);
        if(name(br(T)) == NIL_ATOM)
        {
            (void)UNIFY(1,T,TT,B,NIL_TERM,MAXDEPTH);
            return Result;
        }
        (void)UNI(TT,mkfunc(COMMA_2,stackvar(2)));
    }
   /*NOTREACHED*/
}

LOCAL CLAUSE make_clause(T)
    TERM T;
{
    deref(T);
    if(name(T) == ARROW_2)
        return NewClause(arg1(T),arg2(T) , true);
    else
        return NewClause(T,NIL_TERM , true);
}


GLOBAL void DOASSERT(asserta)
    boolean asserta;           
{                         
    ATOM A;
    CLAUSE CL,C;


    if((A = name(A0))== ARROW_2) 
        A=nameof(son(A0));
    if((system(A) && !aSYSMODE) || class(A)!=NORMP) 
        ERROR(SYSPROCE);
    A=copyatom(A);
    CL = make_clause(A0);
    if(asserta || (C = clause(A)) == NIL_CLAUSE)
    { 
        nextcl(CL) = clause(A);
        clause(A) = CL;
    }
    else 
    { 
        while(nextcl(C) != NIL_CLAUSE)
            C = nextcl(C);
        nextcl(C)=CL; 
        nextcl(CL)=NIL_CLAUSE;
    }
    set_in_use(CL);
}



GLOBAL void FreeClause(CL)
    CLAUSE CL;
/*
** Diese Funktion haengt von der konkreten Implementation
** der abstrakten Datantypen ab
*/
{ 
    TERM T,S;
    int i = _head_offset+1;
    nextcl(CL) = NIL_CLAUSE; /* nicht notwendig, aber sicherer */
    unset_in_use(CL);
    S = CL; 
    T = head(CL);
    if(func_atom(name(T)))
        freeterm(arity(name(T)),son(T),MAXDEPTH);
    T = body(CL);
    while(1)
    {
        ++i;
        if(name(T) == NIL_ATOM)
        {
            freeblock(i,S);
            break;
        }
        else if(name(T) == GOTOT)
        {
            T = val(T);
            freeblock(i,S);
            S = T; i = 0;
            if(S != NIL_TERM)
                continue;
            else
                break;
        }
        else if(func_atom(name(T)))
            freeterm(arity(name(T)),son(T),MAXDEPTH);
        
        next_br(T);
    }
}

LOCAL CLAUSE clausechain=NIL_CLAUSE;   

GLOBAL void retractclauses()
{ 
    CLAUSE CL ;
    while((CL=clausechain) != NIL_CLAUSE)
    {
        clausechain = nextcl(CL);
        FreeClause(CL);
    }
    collect_garbage(false);
}


LOCAL boolean active_clause(CL)
    CLAUSE CL;
/* true, if CL is an active goal */
{
    ENV EE;
    CLAUSE CP;
    ATOM A;
    boolean result = false;

    EE = NIL_ENV;
    while((EE = get_env(EE)) != NIL_ENV)
    {
        if(call(EE) != NIL_TERM && func_atom(A=name(call(EE))))
	{
            if((class(A)==NORMP || var_call(A)) && 
		(rule(EE) == CL))
            { 
                result = true;
                if((WARNFLAG & WARN_RETRACT) == WARN_RETRACT)
                {
                    TERM T;
		    START_WARNING();
                    ws("WARNING: retract active goal:");
                    LISTOUT(head(CL));
                    T = body(CL);
                    while(name(T) != NIL_ATOM)
                    {
                        ws(T == body(CL) ? " :- " : ", ");
                        LISTOUT(T);
                        next_br(T);
                    }
                    ws(". ");
		    END_WARNING();

                }
                CP = clause(A);
                if( CP == CL) 
                { 
                    rule(EE) = DUMMYCL ; 
                    continue; 
                }
                while(CP != NIL_CLAUSE && (nextcl(CP)!= CL))
                    CP = nextcl(CP);
                rule(EE) = CP;
            }
            else if(A== CLAUSE_2 && bct(EE)==CL)
            { 
                /* clause/2 is a  backtrackable built in, thats why
                    bct(EE) is set to BCT with son(BCT)=nextclause */
                result = true;
                /*
                if(WARNFLAG)
                    ws("WARNING: retract active goal (clause)\n");
                */
                bct(EE) = nextcl(CL);
            }
	}
    }
    return result;
}

LOCAL void destroycl(CL)
register CLAUSE CL;
{
    register ATOM A;
    register CLAUSE hcl;
    boolean active;
    A=name(head(CL)); 
    active = active_clause(CL);
    hcl = clause(A);
    unset_in_use(CL);
    if(hcl == CL)
        clause(A)= nextcl(CL);
    else 
    {
        while(nextcl(hcl) != CL) 
            hcl = nextcl(hcl);
        nextcl(hcl) = nextcl(CL);
    }
    if(active)
    {
        nextcl(CL) = clausechain;
        clausechain = CL; 
    }
    else 
        FreeClause(CL);
}



LOCAL TERM GenTerm(CL,B)
    CLAUSE CL;
    TERM B;
{
    TERM T,TT;
    T = ClauseBody(CL,B);
    if(T == NIL_TERM)
        return ClauseHead(CL,B);
    else
    {
        TT = stackvar(2);
        (void)UNI(TT,ClauseHead(CL,B));
        (void)UNI(br(TT),T);
        return mkfunc(ARROW_2,TT);
    }
}

GLOBAL boolean DORETRACT(retractall)
    boolean retractall;
{ 
    CLAUSE CL,NCL;
    ATOM A;
    TERM BASE;
    TRAIL Trail_Top;
    TERM Term_Top;

    BCT = MIN_TERM; inc_term(BCT);

    A=name(A0);
    if(A==ARROW_2) 
        A=nameof(arg1(A0));
    if((system(A) && !aSYSMODE) || class(A)!=NORMP) 
        ARGERROR();
    if( A == REPEAT_0 )
	return retractall;
    CL = clause(A);
    /* now CL is the first clause to check */
    Term_Top = mark_term_stack();
    Trail_Top= mark_trail_stack();

    while(CL != NIL_CLAUSE)
    { 
        BASE = stackvar(nvars(CL)); 
        NCL=nextcl(CL);
        if( ! retractall && UNI(A0,GenTerm(CL,BASE)))
        { 
            destroycl(CL);
            return true;
        }
        else if(retractall && UNIFY(1,A0,head(CL),BE,BASE,MAXDEPTH))
        {
            destroycl(CL);
        }
        rel_term_stack(Term_Top);
        rel_trail_stack(Trail_Top);
        CL = NCL;
    }
    return(retractall);
}


GLOBAL boolean DOCLAUSE()
{ 
    CLAUSE CL;
    TERM T;
    TRAIL Trail_Top;
    TERM Term_Top;


    CL = (CLAUSE)(BCT);
    if(CL == NIL_CLAUSE)
    { 
        ATOM A; 
        A=name(A0);
        if(!func_atom(A))
            ARGERROR();
        if((system(A) && !aSYSMODE) || class(A)!=NORMP) 
            return false;
        CL = clause(A);
    }

    Term_Top = mark_term_stack();
    Trail_Top= mark_trail_stack();

    while(CL != NIL_CLAUSE)
    { 
        TERM Base = stackvar(nvars(CL));

        if(UNI(A0,ClauseHead(CL,Base)))
        {
            T = ClauseBody(CL,Base);
            if(T == NIL_TERM ? UNI(A1,mkatom(TRUE_0)) : UNI(A1,T))
            {
                BCT = (TERM)nextcl(CL);
                return true;
            }
        }
        CL = nextcl(CL);
        rel_term_stack(Term_Top);
        rel_trail_stack(Trail_Top);
    }
    return false;
}

LOCAL void  abolish(A) 
    ATOM A;
{ 
    CLAUSE CL,CL1;
    if((system(A) && !aSYSMODE) || class(A)!=NORMP) 
        return;
    if( A == REPEAT_0 )
	return;
    CL = clause(A);
    while(CL != NIL_CLAUSE)
    { 
        CL1 = CL;
        CL = nextcl(CL); 
        destroycl(CL1); 
    }
}

GLOBAL void  DOABOLISH(argc)
    int argc;
{
    ATOM A;

    if(argc == 1)
    {
        if(isatom(A0))
        {
            register int i;
            for(i=0;i<=MAXARITY;++i)
                if((A = LOOKATOM(name(A0),i,false)) != NIL_ATOM)
                    abolish(A);
        }
        else if((A=atom(A0)) != NIL_ATOM)
            abolish(A);
    }
    else if(argc == 2)
    {   
        CHECKATOM(A0);
        if((A=LOOKATOM(name(A0),INTVALUE(A1),false)) != NIL_ATOM) 
            abolish(A);
    }
    else
        SYSTEMERROR("DOABOLISH");
}


GLOBAL boolean DOCONSULT(FILENAME,reconsult,library)
    TERM FILENAME;  
    boolean reconsult;
    boolean library;
{
    TERM X;
    ATOM A,FILEATOM;
    ATOM LASTA = NIL_ATOM;
    CLAUSE LASTCL;
    ENV EP,CUR_ENV = 0;
    TERM oldinfile;
    CLAUSE CX,CL;
    boolean res = true;
    auto ENV CHOICE;
    auto jmp_buf old_error_label;

    LASTCL = NIL_CLAUSE;

    EP=E;
    CHOICE = CHOICEPOINT;

    if(name(FILENAME) == MINUS_1)
    { 
        FILENAME=arg1(FILENAME); 
        reconsult=true;
    }
    CHECKATOM(FILENAME);

    if(reconsult)
    { 
        A = NIL_ATOM;
        while((A= GetAtom(A)) != NIL_ATOM)
            setnotrc(A); 
    }

    if((FILEATOM = name(FILENAME)) == USER_0)
        FILEATOM = STDIN_0;
    FILEATOM = copyatom(FILEATOM);

    oldinfile=FNAME(inputfile);

    if((inputfile=OpenFile(phy_name(FILEATOM),read_mode))<FIRSTFILE)
    { 
        FileError(CANTOP);
        res = false;
        goto exit_func;
    }
    if(!ISINPUT(inputfile))
        LINELENGTH = CHARPOS = 0;
    ISINPUT(inputfile) = true;
    FLOGNAME(inputfile) = FILEATOM;

    CUR_ENV=newenv();
    base(CUR_ENV)       = NIL_TERM;
    termtop(CUR_ENV)    = mark_term_stack();
    trail(CUR_ENV)      = mark_trail_stack();
    atomtop(CUR_ENV)    = mark_atom_stack();
    stringtop(CUR_ENV)  = mark_string_stack();
    env(CUR_ENV)        = NIL_ENV;
    choice(CUR_ENV)     = NIL_ENV;


    SAVE_ERROR((char *)old_error_label);

    if(CATCH_ERROR())
    {
	goto reset_error_label;
    }

    while(! HALTFLAG && !UserAbort)
    {
        rel_term_stack(termtop(CUR_ENV));
        rel_trail_stack(trail(CUR_ENV));
        rel_atom_stack(atomtop(CUR_ENV));
        rel_string_stack(stringtop(CUR_ENV));
        rel_env_stack(CUR_ENV);
        E = CHOICEPOINT = CUR_ENV;
        BE = base(E);

        if(VERBOSEFLAG)
            if (FILEATOM == STDIN_0) 
                ws("user >");

        A = name(X = READIN());
        if (A == END_0) 
            HALTFLAG = true;
        else if(A == QUESTION_1 || A == ARROW_1)
        {
            LASTA = NIL_ATOM;
            name(X)= CALL_1;  /* ! */
            if( ! EXECUTE(X,NIL_ENV) && 
		(WARNFLAG & WARN_CONSULT) == WARN_CONSULT && A!=ARROW_1)
	    {
		START_WARNING();
                ws("WARNING: goal failed during consult/reconsult\n");
		END_WARNING();
	    }
        }
        else if(A == DCG_2)
        {
	    TERM Y = mkfunc( CALL_1 , X );
            LASTA = NIL_ATOM;
            if( ! EXECUTE(Y,NIL_ENV) && 
		(WARNFLAG & WARN_CONSULT) == WARN_CONSULT)
	    {
		START_WARNING();
                ws("WARNING: dcg rule failed during consult/reconsult\n");
		END_WARNING();
	    }
        }
        else 
        {
            if(A  == ARROW_2) 
                A=nameof(son(X));
            if((system(A) && !aSYSMODE) || class(A) != NORMP)
                ABORT(SYSPROCE);
            A = copyatom(A);
            if(reconsult && !rc(A))
            { 
                setrc(A); 
                abolish(A); 
            }
            /* inline code for assert */
            if((CX = clause(A)) != NIL_CLAUSE)
            {
                if(A != NIL_ATOM && A == LASTA && LASTCL != NIL_CLAUSE) 
                    CX = LASTCL;
                else
                    while((CL=nextcl(CX)) != NIL_CLAUSE)
                        CX=CL;
                nextcl(CX)=CL=make_clause(X);
                if((WARNFLAG & WARN_CONSULT) == WARN_CONSULT
		    && LASTA != A)
                {
		    START_WARNING();
                    ws("WARNING: new clauses for ");
                    ws(tempstring(atomstring(A)));
                    ws("/");
                    ws(itoa(arity(A)));
                    ws("\n");
		    END_WARNING();
                }
            }
            else
                clause(A)=CL=make_clause(X);
            nextcl(CL)=NIL_CLAUSE;
	    set_in_use(CL);
            LASTA = A;
            LASTCL = CL;

	    if(library)
		setlib(A);
	    else
		setnotlib(A);
        }
    }

reset_error_label:
    RESTORE_ERROR((char *)old_error_label);

exit_func:


    HALTFLAG = false;
    if(FNAME(inputfile) != oldinfile)
	CloseFile(inputfile);
    inputfile=OpenFile(oldinfile,read_mode);
    EOF(inputfile)=false;

    /* naechsten 5 evt. nicht notwendig */
    rel_term_stack(termtop(CUR_ENV));
    rel_trail_stack(trail(CUR_ENV));
    rel_atom_stack(atomtop(CUR_ENV));
    rel_string_stack(stringtop(CUR_ENV));
    rel_env_stack(CUR_ENV);

    CHOICEPOINT = CHOICE;

    E=EP; 
    BE= (E == NIL_ENV ? NIL_TERM : base(E));
    if(UserAbort)
        ERROR(ABORTE);
    return res;
}

GLOBAL boolean DOENSURE()
{
    ATOM A;
    int ARITY;
    STRING S;
    string s;
    int i;

    if(!(isatom(A0) && isatom(A1)) || name(A2) != INTT) 
        ARGERROR();
    ARITY= ival(A2);
    if(ARITY < 0 || ARITY > MAXARITY) 
        ARGERROR();
    A = LOOKATOM(name(A1),ARITY,true);
    A = copyatom(A);
    if(ensure(A) || (class(A) == NORMP && clause(A) != NIL_CLAUSE)) 
        return true;
    STARTATOM();
    S = atomstring(name(A0));
    for(i = 0; stringchar(S,i) != '\0' ; ++i)
        ATOMCHAR(stringchar(S,i));
    S = atomstring(name(A1));
    for(i = 0; stringchar(S,i) != '\0' ; ++i)
        ATOMCHAR(stringchar(S,i));
    ATOMCHAR('.');
    s = itoa(ARITY);
    while(*s != '\0')
        ATOMCHAR(*s++);
    ATOMCHAR('\0');
    if(file_exist(NEWATOM) == 0) 
        return false;
    setensure(A);
    (void)DOCONSULT(mkatom(LOOKUP(NEWATOM,0,false,true)),false,true);
    return true;
}

/****************** I N I T I A L I S A T I O N ***********/


LOCAL TERM CURRTERM;
LOCAL int CURRMAX;

LOCAL CLAUSE initclause(N,VARS)
    int N,VARS;
{ 
    CLAUSE CL;
    if(CURRMAX != 0)
        SYSTEMERROR("InitDatabase");
    CL=heapterm(N+_head_offset + 1);
    CURRTERM=head(CL);
    CURRMAX=N+1;
    setnvars(CL,VARS);
    nextcl(CL) = NIL_CLAUSE;
    return CL;
}

LOCAL void setvar(S)
    int S;
{ 
    if (CURRMAX-- <= 1) 
        SYSTEMERROR("InitDatabase");
    name(CURRTERM)=SKELT;
    offset(CURRTERM)=S;
    next_br(CURRTERM);
}

LOCAL void setarg(A,S)
    ATOM A;
    TERM S;
{ 
    if (CURRMAX-- <= 1) 
        SYSTEMERROR("InitDatabase");
    name(CURRTERM)=A;
    son(CURRTERM)=S;
    next_br(CURRTERM);
}

LOCAL void endclause()
{
    if(CURRMAX-- != 1)
        SYSTEMERROR("InitDatabase");
    name(CURRTERM) = NIL_ATOM;
    son(CURRTERM)  = NIL_TERM;
}

LOCAL TERM vars(M,N)
    int M,N;
{ 
    register TERM T;
    T=heapterm(2); 
    name(T)=SKELT; 
    offset(T)=M;
    name(br(T))=SKELT; 
    offset(br(T))=N;
    return T;
}

LOCAL TERM var(N)
    int N;
{ 
    register TERM T;
    T=heapterm(1); 
    name(T)=SKELT; 
    offset(T)=N;
    return T;
}

LOCAL void arithclause(A)
    ATOM A;
{ 
    register TERM P;
    clause(A)=initclause(5,4);
    setarg(A,vars(0,1));
    setarg(EVALUATE_2,vars(2,0));
    setarg(EVALUATE_2,vars(3,1));
    setarg(dCUT_0,NIL_TERM);
    P=heapterm(1); 
    name(P)=A; 
    son(P)=vars(2,3);
    setarg(ACOMP_1,P);
    endclause();
}

GLOBAL CLAUSE ANDG,OR1G,OR2G,IMPG,DUMMYCL;

GLOBAL void Init_Datab()
{ 
    register TERM P;
    register CLAUSE C;

    CURRMAX = 0;

#if DEBUG
    if(DEBUGFLAG)
        out(2,"Init_Datab\n");
#endif
    /* (P,Q):-P,Q. */
    clause(COMMA_2)=ANDG=initclause(3,2);
    setarg(COMMA_2,vars(0,1));
    setvar(0);
    setvar(1);
    endclause();

    /* (P;_):-P. (_;Q):-Q. */
    clause(SEMI_2)=OR1G=initclause(2,2);
    setarg(SEMI_2,vars(0,1));
    setvar(0);
    endclause();

    nextcl(OR1G)=OR2G=initclause(2,2);
    setarg(SEMI_2,vars(0,1));
    setvar(1);
    endclause();

    /* (P->Q):-P,!,Q.  */
    clause(IMPL_2)=IMPG=initclause(4,2);
    setarg(IMPL_2,vars(0,1));
    setvar(0);
    setarg(CUT_0,NIL_TERM);
    setvar(1);
    endclause();

    /* repeat.  repeat:-repeat.  */
    C=clause(REPEAT_0)=initclause(1,0);
    setarg(REPEAT_0,NIL_TERM);
    endclause();
    nextcl(C)=C;

    /* true.  */
    clause(TRUE_0)=initclause(1,0);
    setarg(TRUE_0,NIL_TERM);
    endclause();

    /* not X:-call(X),!,fail.  not _.  */
    clause(NOT_1)=C=initclause(4,1);
    setarg(NOT_1,var(0));
    setarg(CALL_1,var(0));
    setarg(CUT_0,NIL_TERM);
    setarg(FAIL_0,NIL_TERM);
    endclause();
    nextcl(C)=initclause(1,1);
    setarg(NOT_1,var(0));
    endclause();

    /* \+ X:-call(X),!,fail.  \+ _.  */
    clause(NOT1_1)=C=initclause(4,1);
    setarg(NOT1_1,var(0));
    setarg(CALL_1,var(0));
    setarg(CUT_0,NIL_TERM);
    setarg(FAIL_0,NIL_TERM);
    endclause();
    nextcl(C)=initclause(1,1);
    setarg(NOT1_1,var(0));
    endclause();

#if USER_UNIFY
    /* unify(X,X) :-!.  */
    clause(UNIFY_2)=initclause(2,1);
    setarg(UNIFY_2,vars(0,0));
    setarg(CUT_0,NIL_TERM);
    endclause();

    /* X = Y :- unify(X,Y). */

    clause(ISEQ_2)=initclause(2,2);
    setarg(ISEQ_2,vars(0,1));
    setarg(UNIFY_2,vars(0,1));
    endclause();

    /* X \= Y :- unify(X,Y), ! , fail.  _ \= _. */
    clause(ISNEQ_2)=C=initclause(4,2);
    setarg(ISNEQ_2,vars(0,1));
    setarg(UNIFY_2,vars(0,1));
    setarg(CUT_0,NIL_TERM);
    setarg(FAIL_0,NIL_TERM);
    endclause();
    nextcl(C)=initclause(1,2);
    setarg(ISNEQ_2,vars(0,1));
    endclause();


#else
    /* X=X.  */
    clause(ISEQ_2)=initclause(1,1);
    setarg(ISEQ_2,vars(0,0));
    endclause();

    /* X\=X:-!,fail.  _\=_.  */

    clause(ISNEQ_2)=C=initclause(3,1);
    setarg(ISNEQ_2,vars(0,0));
    setarg(CUT_0,NIL_TERM);
    setarg(FAIL_0,NIL_TERM);
    endclause();
    nextcl(C)=initclause(1,2);
    setarg(ISNEQ_2,vars(0,1));
    endclause();
#endif

    /*
     [X,Y|T]:-consult(X),[Y|T].
     [X] :- consult(X).
    */
    P=heapterm(2);
    name(P)=SKELT; 
    offset(P)=0;
    name(br(P))=CONS_2; 
    son(br(P))=vars(1,2);
    clause(CONS_2)=C=initclause(3,3);
    setarg(CONS_2,P);
    setarg(CONSULT_1,var(0));
    setarg(CONS_2,vars(1,2));
    endclause();
    P=heapterm(2);
    name(P)=SKELT; 
    offset(P)=0;
    name(br(P))=NIL_0; 
    son(br(P))=NIL_TERM;
    nextcl(C)=initclause(2,1);
    setarg(CONS_2,P);
    setarg(CONSULT_1,var(0));
    endclause();

    /* call(X):-X.  */
    clause(CALL_1)=initclause(2,1);
    setarg(CALL_1,var(0));
    setvar(0);
    endclause();

    /* retractall(X) :- retract(X) , fail .
       retractall(_). */

    clause(RETALL_1) = initclause(3,1);
    setarg(RETALL_1,var(0));
    setarg(RETRACT_1,var(0));
    setarg(FAIL_0,NIL_TERM);
    endclause();

    nextcl(clause(RETALL_1)) = initclause(1,1);
    setarg(RETALL_1,var(0));
    endclause();


    /* D := E :- $evaluate(X,E),$!,$dass(D,X).  */
    clause(ASSIGN_2)=initclause(4,3);
    setarg(ASSIGN_2,vars(0,1));
    setarg(EVALUATE_2,vars(2,1));
    setarg(dCUT_0,NIL_TERM);
    setarg(DASSIGN_2,vars(0,2));
    endclause();

    /*
     A =:= B :- $evaluate(AR,A),$evaluate(BR,B),$!,$acomp(AR =:= BR).
     etc.
  */
    arithclause(EQ_2);
    arithclause(NE_2);
    arithclause(LT_2);
    arithclause(GT_2);
    arithclause(LE_2);
    arithclause(GE_2);

    /*
    :- assign(stdwarn,stdout).
    % --> $file(stdout,stdwarn)
    */
    C=initclause(1,0);
    setarg(FNAME_2,P = heapterm(2));
    name(br(P)) = STDWRN_0;
    name(P) = STDOUT_0;
    endclause();
    nextcl(C) = clause(FNAME_2);
    clause(FNAME_2) = C;

    DUMMYCL = initclause(0,0);
}


/* end of file */
