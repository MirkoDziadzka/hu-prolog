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
#include "memory.h"
#include "extern.h"


/**********************************************************
*                                                         *
*  STRINGS                                                *
*                                                         *
**********************************************************/

#if QUICK_BUT_DIRTY
GLOBAL STRING STRINGHTOP = MIN_STRING; /* erstes freies Feld */
GLOBAL STRING STRINGSTOP = MAX_STRING; /* letztes belegtes Feld */
#endif
#if !QUICK_BUT_DIRTY
LOCAL STRING STRINGHTOP = MIN_STRING; /* erstes freies Feld */
LOCAL STRING STRINGSTOP = MAX_STRING; /* letztes belegtes Feld */
#endif

GLOBAL STRING heapstring(s)
    string s;
{ 
    STRING P;
    STRING Q;
    Q=P=STRINGHTOP;
    while((stab(P++) = *s++) != '\0')
        ;
    if(P >= STRINGSTOP) 
        ABORT(aSTRINGSPACEE);
    STRINGHTOP=P;
    return Q;
}

GLOBAL STRING stackstring(s)
    register string s;
{ 
    register STRING P;
    P= (STRINGSTOP - (strlen(s)+1));
    if(STRINGHTOP >= P)
        ABORT(aSTRINGSPACEE);
    STRINGSTOP = P;
    while ((stab(P++) = *s++) != '\0')
        ;
    return STRINGSTOP;
}

#ifndef stringchar
GLOBAL char stringchar(S,i)
    STRING S;
    int i;
{
    return stab(S+i);
}
#endif


#ifndef init_string_stack
GLOBAL void init_string_stack()
{
    STRINGSTOP = MAX_STRING;
}
#endif

#ifndef rel_string_stack
GLOBAL void rel_string_stack(S)
    STRING S;
{
    STRINGSTOP = S;
}
#endif

#ifndef mark_string_stack
GLOBAL STRING mark_string_stack()
{
    return STRINGSTOP;
}
#endif

#ifndef tempstring
GLOBAL string tempstring(S)
    register STRING S;
{
    static char buffer[ MAX_NAME_LEN+1 ];
    register int i;
    for(i=0;(buffer[i] = stringchar(S,i)) != '\0';++i)
        if(i > MAX_NAME_LEN)
            ERROR(ATOMSPACEE);
    return buffer;
}
#endif

/**********************************************************
*                                                         *
*  ATOMS                                                  *
*                                                         *
**********************************************************/

#if QUICK_BUT_DIRTY
GLOBAL ATOM ATOMHTOP = LAST_ATOM; /* letztes belegtes Feld */
GLOBAL ATOM ATOMSTOP = MAX_ATOM; /* letztes belegtes Feld */
#endif
#if !QUICK_BUT_DIRTY
LOCAL ATOM ATOMHTOP = LAST_ATOM; /* letztes belegtes Feld */
LOCAL ATOM ATOMSTOP = MAX_ATOM; /* letztes belegtes Feld */
#endif

#if USER
LOCAL boolean init = true;
GLOBAL void reserve_atom(A)
    ATOM A;
{
    if(!init)
        SYSTEMERROR("reserve_atom");
    if(A >= ATOMHTOP)
        ATOMHTOP = A;
}
#endif

GLOBAL ATOM heapatom()
{
#if USER
    init = false;
#endif
    if(inc_atom(ATOMHTOP) >= ATOMSTOP)
        ABORT(ATOMSPACEE);
    return ATOMHTOP;
}

GLOBAL ATOM stackatom()
{
    if(dec_atom(ATOMSTOP) <= ATOMHTOP)
        ABORT(ATOMSPACEE);
    return ATOMSTOP;
}

#ifndef is_heap_atom
GLOBAL boolean is_heap_atom(A)
    register ATOM A;
{
#if DEBUG
    if(A == NIL_ATOM)
        SYSTEMERROR("is_heap_atom - bad arg");
#endif
    return (A <= ATOMHTOP);
}
#endif

#ifndef is_stack_atom
GLOBAL boolean is_stack_atom(A)
    register ATOM A;
{
#if DEBUG
    if(A == NIL_ATOM)
        SYSTEMERROR("is_stack_atom - bad arg");
#endif
    return (A >= ATOMSTOP);
}
#endif

#ifndef init_atom_stack
GLOBAL void init_atom_stack()
{
    ATOMSTOP = MAX_ATOM;
}
#endif

#ifndef rel_atom_stack
GLOBAL void rel_atom_stack(A)
    ATOM A;
{
    ATOMSTOP = A;
}
#endif

#ifndef mark_atom_stack
GLOBAL ATOM mark_atom_stack()
{
    return ATOMSTOP;
}
#endif

#ifndef norm_atom
GLOBAL boolean norm_atom(A)
    ATOM A;
{
    return (A >= NORMATOM);
}
#endif

#ifndef func_atom
GLOBAL boolean func_atom(A)
    ATOM A;
{
    return (A >= FUNCNAME);
}
#endif

#ifndef get_stack_atom
GLOBAL ATOM get_stack_atom(A)
    register ATOM A;
{
    if( A == NIL_ATOM)
        A = ATOMSTOP;
    else
        inc_atom(A);
    if(A == MAX_ATOM)
        A = NIL_ATOM;
    return A;
}
#endif

#ifndef is_integer
GLOBAL boolean is_integer(A)
    register ATOM A;
{
    return A == INTT 
#if LONGARITH
        || A == LONGT
#endif
    ;
}
#endif

#ifndef is_number
GLOBAL boolean is_number(A)
    register ATOM A;
{
    return A == INTT
#if LONGARITH
        || A == LONGT 
#endif
#if REALARITH
        || A == REALT
#endif
    ;
}
#endif

/**********************************************************
*                                                         *
*  TERMS                                                  *
*                                                         *
**********************************************************/

#if QUICK_BUT_DIRTY
GLOBAL TERM HEAPTOP = MIN_TERM; /* erstes freies Feld */
GLOBAL TERM GLOTOP = MAX_TERM; /* erstes freies Feld */
#endif
#if !QUICK_BUT_DIRTY
LOCAL TERM HEAPTOP = MIN_TERM; /* erstes freies Feld */
LOCAL TERM GLOTOP = MAX_TERM; /* erstes freies Feld */
#endif

LOCAL TERM freelist[MAXARITY+1];  /* chain of disposed nodes */

LOCAL void init_freelist()
{
    register int i;
    for(i = 0; i<= MAXARITY;++i)
        freelist[i] = NIL_TERM;
}

#ifndef br
GLOBAL TERM br(T)
    TERM T;
{
    return T + TERM_UNIT;
}
#endif

#ifndef n_brother
GLOBAL TERM n_brother(T,N)
    TERM T;
    int N;
{
    return T + N * TERM_UNIT; 
}
#endif

#define res_stack(X,N)    {if(( X = (GLOTOP -= (N)*TERM_UNIT)) <= HEAPTOP) \
                               ABORT(LOCALSPACEE); }
GLOBAL TERM stackterm(N)
    register int N;
{ 
    register TERM X;
    if(N == 0) 
        return NIL_TERM;
    res_stack(X,N);
    return X;
}


GLOBAL TERM heapterm(N)
    register int N;
{ 
    register TERM T;
    if (N > MAXARITY) 
        SYSTEMERROR("heapterm");
    if(N == 0) 
        return NIL_TERM;
    if (freelist[N] != NIL_TERM)
    { 
        T=freelist[N]; 
        freelist[N]=son(T); 
    }
    else
    {
        T=HEAPTOP;
        HEAPTOP += N*TERM_UNIT;
        if(GLOTOP <= HEAPTOP)
	{
	    HEAPTOP = T;
            ABORT(LOCALSPACEE);
	}
    }
    return T;
}

GLOBAL void freeblock(N,T)
    register int N;
    register TERM T;
{ 

    if (N==0) return;
    if (N > MAXARITY) 
        SYSTEMERROR("freeblock");

    son(T)=freelist[N]; 
    freelist[N]=T;
}

#ifndef init_term_stack
GLOBAL void init_term_stack()
{
    GLOTOP = MAX_TERM;
}
#endif

#ifndef mark_term_stack
GLOBAL TERM mark_term_stack()
{
    return GLOTOP;
}
#endif

#ifndef rel_term_stack
GLOBAL void rel_term_stack(T)
     TERM T;
{
    GLOTOP = T;
}
#endif

#ifndef is_heap_term
GLOBAL boolean is_heap_term(T)
    TERM T;
{
    return T < HEAPTOP;
}
#endif

#ifndef is_stack_term
GLOBAL boolean is_stack_term(T)
    TERM T;
{
    return T >= GLOTOP;
}
#endif

#ifndef is_older_term
GLOBAL boolean is_older_term(Older,Younger)
    TERM Older,Younger;
{
    return Older > Younger;
}
#endif

GLOBAL TERM get_stack_term(T)
    register TERM T;
{
    /* NIL_TERM < T <= GLOTOP */
    if(T == NIL_TERM)
        return GLOTOP;
    else if(T == MAX_TERM)
        return NIL_TERM;
    else
        return inc_term(T);
}


GLOBAL void collect_garbage(abort)
    boolean abort;
/* reclaim heapnodes if possible */
{
    register TERM T,LASTT;
    register int ar;
    /* Alle freien Terme als frei kennzeichnen */
    for(ar=1;ar<=MAXARITY;++ar)
    {
	for(T = freelist[ar] ; T != NIL_TERM ; T = son(T))
        {
	    register int j;
	    register TERM TT;
	    TT = T;
	    for(j=0, TT = T; j < ar ; ++j, next_br(TT))
		name(TT) = GC_MARKER;
        }
    }
    /* HEAPTOP zuruecksetzen */

    dec_term(HEAPTOP); 
    while(name(HEAPTOP) == GC_MARKER)
	dec_term(HEAPTOP); 
    inc_term(HEAPTOP);

    /* Freigegebene Bloecke ausketten und frei-Markierung ruecksetzen */
    for(ar=1;ar<=MAXARITY;++ar)
    {
	LASTT = T = freelist[ar]; 
	while(T != NIL_TERM) 
        {
	    register int j;
	    register TERM TT;
	    TT = T;
	    for(j=0, TT = T; j < ar ; ++j, next_br(TT))
		name(TT) = NIL_ATOM;
	    if(!is_heap_term(T))
	    {
		/* T ausketten */
		if(LASTT == T)
		    freelist[ar] = T = LASTT = son(T);
		else
		    son(LASTT) = T = son(T);
	    }
	    else
		T = son(LASTT = T);
        }
    }

    if(abort && HEAPTOP >= GLOTOP)
        ABORT(LOCALSPACEE);
}

GLOBAL TERM stackvar(N)
    register int N;
{
    register TERM T;
    register TERM Result;

    res_stack(T = Result,N);
    while(N-- != 0)
    {
        name(T) = UNBOUNDT;
        inc_term(T);
    }
    return Result;
}

GLOBAL TERM mkfreevar()
{ 
    register TERM T;
    res_stack(T,1);
    name(T) = UNBOUNDT;
    return T;
}

GLOBAL TERM arg1(T)
    register TERM T;
{ 
    T=son(T); 
    deref(T); 
    return T; 
}

GLOBAL TERM arg2(T)
    register TERM T;
{ 
    T=son(T);
    next_br(T); 
    deref(T); 
    return T; 
}

GLOBAL TERM arg(N,T)
    register TERM T;
    register int N;
{
    T=son(T); 
    while(--N > 0)
	next_br(T); 
    deref(T); 
    return T; 
}


GLOBAL TERM mkfunc(A,T)
    ATOM A;
    TERM T;
{ 
    register TERM X;
    res_stack(X,1);
    name(X)=A; 
    son(X)=T;
    return X;
}

GLOBAL TERM mkatom(N)
    ATOM N;
{ 
    register TERM X;
    res_stack(X,1);
    name(X)=N; 
    son(X)=NIL_TERM;
    return X;
}

GLOBAL TERM mkint(N)
    int N;
{ 
    register TERM X;
    res_stack(X,1);
    name(X)=INTT; 
    ival(X)=N;
    return X;
}

/*VARARGS*/
GLOBAL TERM mk2sons(NAM1,SON1,NAM2,SON2)
    ATOM NAM1,NAM2;
    TERM SON1,SON2;
{ 
    register TERM T;
    res_stack(T,2);
    name(br(T))=NAM2; 
    son(br(T))=SON2;
    name(T)=NAM1; 
    son(T)=SON1;
    return T;
}

/**********************************************************
*                                                         *
*  TRAIL                                                  *
*                                                         *
**********************************************************/

#if QUICK_BUT_DIRTY
GLOBAL TRAIL TRAILEND = MIN_TRAIL; /* erstes freies */
#endif
#if !QUICK_BUT_DIRTY
LOCAL TRAIL TRAILEND = MIN_TRAIL; /* erstes freies */
#endif

GLOBAL void trailterm(TT)
    TERM TT;
{
    boundvar(TRAILEND) = TT;
    inc_trail(TRAILEND);
    if(TRAILEND >= MAX_TRAIL)
        ABORT(TRAILSPACEE);
}

#ifndef init_trail_stack
GLOBAL void init_trail_stack()
{
    TRAILEND = MIN_TRAIL;
}
#endif

#ifndef mark_trail_stack
GLOBAL TRAIL mark_trail_stack()
{
    return TRAILEND;
}
#endif

#ifndef rel_trail_stack
GLOBAL void rel_trail_stack(T)
    register TRAIL T;
{
    register TRAIL newtrail = T;
    while(T < TRAILEND)
    {
        name(boundvar(T)) = UNBOUNDT;
        inc_trail(T);
    }
    TRAILEND = newtrail;
}
#endif

/**********************************************************
*                                                         *
*  ENV                                                    *
*                                                         *
**********************************************************/

#if QUICK_BUT_DIRTY
GLOBAL ENV   ENVTOP   = MIN_ENV; /* letztes belegtes */
#endif
#if !QUICK_BUT_DIRTY
LOCAL ENV   ENVTOP   = MIN_ENV; /* letztes belegtes */
#endif

GLOBAL ENV newenv()
{
    if(inc_env(ENVTOP) >= MAX_ENV)
        ABORT(FRAMESPACEE);
    return ENVTOP;
}

#ifndef init_env_stack
GLOBAL void init_env_stack()
{
    ENVTOP = MIN_ENV;
}
#endif

#ifndef mark_env_stack
GLOBAL ENV mark_env_stack()
{
    return ENVTOP;
}
#endif

#ifndef rel_env_stack
GLOBAL void rel_env_stack(EE)
    ENV EE;
{
    ENVTOP = EE;
}
#endif

#ifndef is_older_env
GLOBAL boolean is_older_env(Older,Younger)
    ENV Older,Younger;
{
    return Older < Younger;
}
#endif

GLOBAL ENV get_env(EE)
    register ENV EE;
{
    if(EE == NIL_ENV)
        EE = MIN_ENV;
    else if(EE == ENVTOP)
        EE = NIL_ENV;
    else inc_env(EE);
    return EE;
}

/**********************************************************
*                                                         *
*  NUMBERS                                                *
*                                                         *
**********************************************************/

#if REALARITH
LOCAL union
{ 
    REAL r; 
    int ir[REALSIZE]; 
} ri;
#endif
#if LONGARITH
LOCAL union
{ 
    LONG l; 
    int il[LONGSIZE]; 
} li;
#endif

#if REALARITH
GLOBAL TERM mkreal(R)
    REAL R;
{ 
    TERM T;
    int I;
    TERM TT;
    ri.r=R;
    res_stack(T = TT,REALSIZE);
    for(I=0;I<REALSIZE;I++)
    { 
        name(T) = INTT ; 
        ival(T) = ri.ir[I];
        next_br(T);
    }
    return mkfunc(REALT,TT);
}

GLOBAL REAL realval(T)
    TERM T;
{ 
    int I;
    if(name(T)!=REALT) ERROR(ARGE);
    T=son(T);
    for(I=0; I<REALSIZE; I++)
    { 
        if (name(T)!=INTT) ERROR(ARGE);
        ri.ir[I]=ival(T); 
        next_br(T);
    }
    return ri.r;
}
#endif

#if LONGARITH
GLOBAL TERM mklong(L)
    LONG L;
{ 
    TERM T,TT; 
    int I;
    li.l=L;
    res_stack(TT = T ,LONGSIZE);
#if MSC /* Fehler im MSC 5.1 Compiler */
    name(T) = INTT ; 
    ival(T) = li.il[0] ; 
    next_br(T);
    name(T) = INTT ; 
    ival(T) = li.il[1] ;
#endif
#if !MSC
    for (I=0; I<LONGSIZE; I++)
    { 
        name(T) = INTT ; 
        ival(T) = li.il[I];
        next_br(T);
    }
#endif
    return mkfunc(LONGT,TT);
}

GLOBAL LONG longval(T)
    TERM T;
{ 
    register int I;
    if(name(T)!=LONGT) ERROR(ARGE);
    T=son(T);
#if MSC /* Fehler im MSC 5.1 Compiler */
    if(name(T) != INTT) ERROR(ARGE);
    li.il[0] = ival(T); 
    next_br(T);
    if(name(T) != INTT) ERROR(ARGE);
    li.il[1] = ival(T);
#endif
#if ! MSC
    for(I=0; I<LONGSIZE; I++)
    { 
        if (name(T)!=INTT) ERROR(ARGE);
        li.il[I]=ival(T); 
        next_br(T);
    }
#endif
    return li.l;
}
#endif

/**********************************************************
*                                                         *
*  STATISTICS                                             *
*                                                         *
**********************************************************/

LOCAL long TOTAL;

LOCAL void wtotal(S,MAX)
string S;
card MAX;
{ 
    ws(S); 
    ws(ltoa((long)MAX)); 
    TOTAL=MAX; 
}

LOCAL void wpercent(S,N)
    string S;
    card N;
{ 
    ws(S); 
    ws(ltoa((long)N));
    ws(" ("); 
    ws(ltoa(((long)N*100L)/TOTAL));
    ws("%)");
}

/* evaluable predicate stats */
GLOBAL void DOSTATS ()
{ 
    card RN; 
    TERM T;
    int I;
    collect_garbage(false);
    ws("\nProlog Execution Statistics:\n");
    RN=0;
    for (I=0;I<=MAXARITY;I++)
    { 
        T=freelist[I]; 
        while(T != NIL_TERM) { 
            RN+=I; 
            T=son(T); 
        } 
    }

    wtotal("\nNodes: ",N_OF_TERMS);
    wpercent(" Stack: ",(card)(MAX_TERM-GLOTOP)/TERM_UNIT);
    wpercent(" Heap: ",(card)(HEAPTOP-MIN_TERM)/TERM_UNIT);
    wpercent(" Released: ",RN);

    wtotal("\nAtoms: ",N_OF_ATOMS);
    wpercent(" Stack: ",(card)(MAX_ATOM-ATOMSTOP)/ATOM_UNIT);
    wpercent(" Heap: ",(card)(ATOMHTOP-MIN_ATOM)/ATOM_UNIT);

    wtotal("\nStrings: ",N_OF_STRINGS);
    wpercent(" Stack: ",(card)(MAX_STRING-STRINGSTOP)/STRING_UNIT);
    wpercent(" Heap: ",(card)(STRINGHTOP-MIN_STRING)/STRING_UNIT);

    wtotal("\nEnvironments: ",N_OF_ENVS);
    wpercent(" Used: ",(card)(ENVTOP-MIN_ENV)/ENV_UNIT);

    wtotal("\nTrail: ",N_OF_TRAILS);
    wpercent(" Used: ",(card)(TRAILEND-MIN_TRAIL)/TRAIL_UNIT);

    ws("\n");
}

#if SAVE
/*  SAVE & RESTORE */
#if MS_DOS
#include <fcntl.h>
#endif

#ifndef O_BINARY
# define O_BINARY 0	
#endif
#ifndef O_WRONLY
#define O_WRONLY 1
#endif
#ifndef O_RDONLY
#define O_RDONLY 2
#endif

#define HEADERSIZE 196

static char FileHeader[HEADERSIZE];
#define concat(st) for(s=st ; *s && index < HEADERSIZE; ++s,++index) \
			FileHeader[index] = *s
LOCAL void mkheader()
{
    int index = 0;
    char *s;
    FileHeader[0] = '\0';
    concat(": Bourne Shell\n# ");
    concat(version);
    concat("# MAGIC = ");
    concat(ltoa(MAGIC_NUMBER));
    concat("\nprolog -r $0 $@\n");
    concat("exit $?\n");
    concat("# --- core image after this point ---\n");
    if(index >= HEADERSIZE)
	SYSTEMERROR("types.c - mkheader() - header to big");
    while(index < HEADERSIZE)
    {
	FileHeader[index++] = '\n';
	if(index < HEADERSIZE)
	    FileHeader[index++] = '\f';
    }
}
#undef concat

LOCAL ERRORTYPE result;

LOCAL int checksum(from , len)
    register char * from;
    register card len;
{
    register int chksum = 0;

    for( ; len > 0 ; --len, ++from)
	chksum += *from;
    return (int)(chksum >> 8);
}

LOCAL void doio(fileptr, save , from , len )
    int fileptr;
    boolean save;
    char * from;
    card len;
{
    int chksum;

    if((save ? write : read )(fileptr,from,len) != len)
	if(result  == NOERROR)
	    result = IOERROR;
    
    chksum = checksum(from,len);
    if(save)
    {
	if(write(fileptr,(char *)&chksum,sizeof(chksum)) != sizeof(chksum))
	    if(result  == NOERROR)
		result = IOERROR;
    }
    else
    {
	int old;
	if(read(fileptr,(char *)&old, sizeof(old)) != sizeof(old))
	    if(result  == NOERROR)
		result = IOERROR;
	if(chksum != old)
	    if(result  == NOERROR)
		result = BAD_CHKSUM;
    }
}

#define handle_var(X)	doio(fileptr,save,(char *)&X , sizeof(X))
#define handle_ff(F)	doio(fileptr,save,(char *)F  , sizeof(F))
#define handle_vf(first_adr,last_adr)  doio(fileptr,save, \
	(char*)(first_adr),(card)((char*)(last_adr) - (char*)(first_adr)))

GLOBAL ERRORTYPE save_restore(save,filename)
    boolean save;
    char *filename;
{
    int fileptr;
    long magic;

    result = NOERROR;

    if(save)
    {
	collect_garbage(false);
	close(creat(filename,0777));
    }

    fileptr = save ? open(filename,O_WRONLY | O_BINARY) :
		     open(filename,O_RDONLY | O_BINARY);

    if(fileptr < 0)
	return (save ? CANTCR : CANTOP);

    if(save)
    {
	if(write(fileptr,FileHeader,HEADERSIZE) != HEADERSIZE)
	    result = IOERROR;
    }
    else
    {
	if(lseek(fileptr,(long)HEADERSIZE,0) == (long)-1)
	    result = IOERROR;
    }

    magic = MAGIC_NUMBER;

    handle_var(magic);

    if(magic != MAGIC_NUMBER)
        return BAD_MAGIC;

    handle_var(ATOMHTOP);
    handle_var(STRINGHTOP);
    handle_var(HEAPTOP);

    handle_ff(HASHTAB);
    handle_ff(freelist);

#if POINTERMODE
    handle_vf(&_atomtab[0], &_atomtab[ATOMHTOP + ATOM_UNIT]);
    handle_vf(MIN_STRING, STRINGHTOP + STRING_UNIT);
    handle_vf(MIN_TERM , HEAPTOP + TERM_UNIT);
#endif
#if WORDMODE || BYTEMODE
    /* ATOMTAB */
    handle_vf(&_a_CLAUSE[0],   &_a_CLAUSE[ ATOMHTOP+ ATOM_UNIT ]);
    handle_vf(&_a_ARITY[0],    &_a_ARITY[ ATOMHTOP+ ATOM_UNIT ]);
    handle_vf(&_a_STRING[0],   &_a_STRING[ ATOMHTOP+ ATOM_UNIT ]);
    handle_vf(&_a_NEXTATOM[0], &_a_NEXTATOM[ ATOMHTOP+ ATOM_UNIT ]);
    handle_vf(&_a_CHAINATOM[0],&_a_CHAINATOM[ ATOMHTOP+ ATOM_UNIT ]);
    handle_vf(&_a_BIT2[0],     &_a_BIT2[ ATOMHTOP+ ATOM_UNIT ]);
    handle_vf(&_a_BIT1[0],     &_a_BIT1[ ATOMHTOP+ ATOM_UNIT ]);
    /* STRINGTAB */
    handle_vf(&_STRINGTAB[0], &_STRINGTAB[ STRINGHTOP + STRING_UNIT]);
    /* TERMTAB */
    handle_vf(&_t_NAME[0] , &_t_NAME[ HEAPTOP + TERM_UNIT ]);
    handle_vf(&_t_VALUE[0] , &_t_VALUE[ HEAPTOP + TERM_UNIT ]);
#endif
#if !(POINTERMODE || WORDMODE || BYTEMODE)
Sorry, but this is not implemented yet
#endif

    /* FLAGS */
    handle_var(TRACING) ; handle_var(SPYTRACE); handle_var(ECHOFLAG);
    handle_var(DEBUGFLAG);handle_var(PROTFLAG); handle_var(OCHECK);
    handle_var(WARNFLAG); handle_var(aSYSMODE); handle_var(VERBOSEFLAG);
    handle_var(xWINDOW_ON);

    (void)close(fileptr);

    return result;
}
#endif

GLOBAL void Init_Types()
{
#if DEBUG
    if(DEBUGFLAG)
        out(2,"Init_Types\n");
#endif
    init_freelist();
#if SAVE
    mkheader();
#endif
}


/* end of file */
