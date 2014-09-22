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


#ifndef __TYPES_H_INCLUDED__
#define __TYPES_H_INCLUDED__	1

#include "config.h"
#include "misc.h"
#include "memory.h"


#if MSC
#define FAR	far
#endif
#if !MSC
#define FAR
#endif

typedef card boolean;
#define true     1
#define false    0

typedef char *string;

#if BYTEMODE || WORDMODE
typedef card STRING;
typedef card ATOM;
typedef card TERM;
typedef card CLAUSE;
typedef card TRAIL;
typedef card ENV;
#endif
#if POINTERMODE
typedef card ATOM;
typedef struct _TRAIL 	*TRAIL;
typedef struct _NODE 	*TERM;
typedef struct _NODE 	*CLAUSE;
typedef struct _ENV 	*ENV;
#if !QUICK_BUT_DIRTY
typedef struct _STRING 	*STRING;
#endif
#if QUICK_BUT_DIRTY
typedef char *STRING;
#endif

struct _ENV
{
    TERM   e_call,e_base,e_termtop,e_bct;
    ENV    e_env,e_choice;
    ATOM   e_atom;
    STRING e_string;
    TRAIL  e_trail;
    CLAUSE e_rule;
    card   e_callkind;
};

struct _NODE
{
    ATOM t_name;
    union
    {
	TERM t_son;
	TERM t_val;
	int  t_offset;
	int  t_ival;
    } t_union;
};

struct _ATOM
{
    STRING	a_string;
    int 	a_arity;
    CLAUSE	a_clause;
    ATOM	a_nextatom;
    ATOM	a_chainatom;
    bit16	a_bit1,a_bit2;
};

#if !QUICK_BUT_DIRTY
struct _STRING
{
    char	s_char;
};
#endif

struct _TRAIL
{
    TERM 	t_term;
};

#endif

/*
**  access definition
*/

#if BYTEMODE
#	define STRING_UNIT      1
#	define ATOM_UNIT  	sizeof(card)
#	define TERM_UNIT  	sizeof(card)
#	define ENV_UNIT	  	sizeof(card)
#	define TRAIL_UNIT 	sizeof(card)
#endif
#if POINTERMODE || WORDMODE
#	define STRING_UNIT      1
#	define ATOM_UNIT 	1
#	define TERM_UNIT 	1
#	define ENV_UNIT	 	1
#	define TRAIL_UNIT 	1
#endif
#define inc_string(i)     (i+=STRING_UNIT)
#define inc_atom(i)       (i+=ATOM_UNIT)
#define inc_term(i)       (i+=TERM_UNIT)
#define inc_env(i)        (i+=ENV_UNIT)
#define inc_trail(i)      (i+=TRAIL_UNIT)
#define dec_string(i)     (i-=STRING_UNIT)
#define dec_atom(i)       (i-=ATOM_UNIT)
#define dec_term(i)       (i-=TERM_UNIT)
#define dec_env(i)        (i-=ENV_UNIT)
#define dec_trail(i)      (i-=TRAIL_UNIT)
#if WORDMODE 
#define external(t,f,i)     extern t f[(i)+1]
#define declare(t,f,i)    t f[(i)+1]
#define acc(t,f,i)        (*((t *)(&(f[i]))))
#define farexternal(t,f,i)     extern t FAR f[(i)+1]
#define fardeclare(t,f,i)    t FAR f[(i)+1]
#define faracc(t,f,i)        (*((t FAR *)(&(f[i]))))
#endif
#if BYTEMODE 
#define external(t,f,i)     extern char f[((i)+1)*sizeof(t)]
#define declare(t,f,i)    char f[(i+1)*sizeof(t)]
#define acc(t,f,i)        (*((t *)(&(f[i]))))
#define farexternal(t,f,i)     extern char FAR f[((i)+1)*sizeof(t)]
#define fardeclare(t,f,i)    char FAR f[(i+1)*sizeof(t)]
#define faracc(t,f,i)        (*((t FAR *)(&(f[i]))))
#endif
#if POINTERMODE
#define external(t,f,i)     extern t f[(i)+1]
#define declare(t,f,i)      t f[(i)+1]
#endif

/*
** STRING
*/

#if BYTEMODE || WORDMODE
#define NIL_STRING	0
#define MIN_STRING	(NIL_STRING + STRING_UNIT)
farexternal(char,_STRINGTAB,N_OF_STRINGS);
#define stab(S)	_STRINGTAB[S]
#endif
#if POINTERMODE
#define NIL_STRING	((STRING)0)
#if !QUICK_BUT_DIRTY
external(struct _STRING,_stringtab, N_OF_STRINGS);
#define MIN_STRING	(&_stringtab[0])
#define stab(S)	((S)->s_char)
#endif
#if QUICK_BUT_DIRTY
external(char ,_stringtab, N_OF_STRINGS);
#define MIN_STRING	(&_stringtab[0])
#define stab(S)	(*(S))
#endif
#endif
PROTOTYPE(char stringchar,(STRING,int ));
PROTOTYPE(void init_string_stack,(void));
PROTOTYPE(STRING mark_string_stack,(void));
PROTOTYPE(void rel_string_stack,(STRING));
PROTOTYPE(STRING stackstring,(string));
PROTOTYPE(STRING heapstring,(string));
PROTOTYPE(string tempstring,(STRING));
#if QUICK_BUT_DIRTY
IMPORT STRING STRINGSTOP;
#define stringchar(S,i)	(stab((S)+(i)*STRING_UNIT))
#define init_string_stack()	(STRINGSTOP = MAX_STRING)
#define mark_string_stack()	(STRINGSTOP+0)
#define rel_string_stack(S)	(STRINGSTOP = (S))
#endif

/*
** ATOM
*/

#if BYTEMODE || WORDMODE
#define atomstring(atom)         faracc(STRING,_a_STRING,atom)
#define arity(atom)              acc(int ,_a_ARITY,atom)
#define clause(atom)             acc(CLAUSE,_a_CLAUSE,atom)
#define nextatom(atom)           faracc(ATOM,_a_NEXTATOM,atom)
#define chainatom(atom)          faracc(ATOM,_a_CHAINATOM,atom)

farexternal(STRING,_a_STRING, N_OF_ATOMS);
external(int ,_a_ARITY, N_OF_ATOMS);
external(CLAUSE,_a_CLAUSE, N_OF_ATOMS);
farexternal(ATOM,_a_NEXTATOM, N_OF_ATOMS);
farexternal(ATOM,_a_CHAINATOM, N_OF_ATOMS);

/* weiter Komponennten als Bitfelder */

#define bit1(atom)               faracc(bit16,_a_BIT1,atom)
#define bit2(atom)               faracc(bit16,_a_BIT2,atom)
farexternal(bit16,_a_BIT1, N_OF_ATOMS);
farexternal(bit16,_a_BIT2, N_OF_ATOMS);
#endif
#if POINTERMODE
external(struct _ATOM , _atomtab, N_OF_ATOMS);

#define atomstring(atom)         (_atomtab[atom].a_string)
#define arity(atom)              (_atomtab[atom].a_arity)
#define clause(atom)             (_atomtab[atom].a_clause)
#define nextatom(atom)           (_atomtab[atom].a_nextatom)
#define chainatom(atom)          (_atomtab[atom].a_chainatom)
#define bit1(atom)               (_atomtab[atom].a_bit1)
#define bit2(atom)               (_atomtab[atom].a_bit2)
#endif

#define NIL_ATOM	0
#define MIN_ATOM	(NIL_ATOM + ATOM_UNIT)

/* 4 Bits */
#define class(atom)              (bit1(atom) & 0x000f)
#define setclass(atom,n)         (bit1(atom)=(bit1(atom)&0xfff0) | n)

typedef card    CLASS;
#define NORMP   0
#define EVALP   1
#define BTEVALP 2
#define VARP    3
#define CUTP    4
#define ANDP    5
#define ORP     6
#define ARITHP  7
#define NUMBP   8
#define GOTOP   9
#if HIGHER_ORDER
#define VARCP	10
#endif

/* 3 Bits */
#define oclass(atom)             ((bit1(atom) & 0x0070) >> 4)
#define setoclass(atom,n)        (bit1(atom)=(bit1(atom)&0xff8f)|(n<<4))

typedef card    OCLASS;
#define NONO    0
#define FXO     1
#define FYO     2
#define XFO  	3
#define YFO  	4
#define XFXO 	5
#define XFYO 	6
#define YFXO 	7

#define lib(atom)                (bit1(atom) & 0x0080)
#define setlib(atom)             (bit1(atom) |= 0x0080)
#define setnotlib(atom)          (bit1(atom) &= 0xff7f)

#define rc(atom)                 (bit1(atom) & 0x0100)
#define setrc(atom)              (bit1(atom) |= 0x0100)
#define setnotrc(atom)           (bit1(atom) &= 0xfeff)

#define spy(atom)                (bit1(atom) & 0x0200)
#define setspy(atom)             (bit1(atom) |= 0x0200)
#define setnotspy(atom)          (bit1(atom) &= 0xfdff)

#define system(atom)             (bit1(atom) & 0x0400)
#define setsystem(atom)          (bit1(atom) |= 0x0400)
#define setnotsystem(atom)       (bit1(atom) &= 0xfbff)

#define private(atom)            (bit1(atom) & 0x0800)
#define setprivate(atom)         (bit1(atom) |= 0x0800)
#define setnotprivate(atom)      (bit1(atom) &= 0xf7ff)

#define ensure(atom)             (bit1(atom) & 0x1000)
#define setensure(atom)          (bit1(atom) |= 0x1000)
#define setnotensure(atom)       (bit1(atom) &= 0xefff)

#define hide(atom)               (bit1(atom) & 0x2000)
#define sethide(atom)            (bit1(atom) |= 0x2000)
#define setnothide(atom)         (bit1(atom) &= 0xdfff)

#define first(atom)              (bit1(atom) & 0x4000)
#define setfirst(atom)           (bit1(atom) |= 0x4000)
#define setnotfirst(atom)        (bit1(atom) &= 0xbfff)

#define arithbi(atom)            (bit1(atom) & 0x8000)
#define setarithbi(atom)         (bit1(atom) |= 0x8000)
#define setnotarithbi(atom)      (bit1(atom) &= 0x7fff)

/* >= 11 Bits */
#define oprec(atom)              ((int)bit2(atom)+0)
#define setoprec(A,Value)	 (bit2(A) = (card)(Value))

PROTOTYPE(ATOM heapatom,(void));
PROTOTYPE(ATOM stackatom,(void));

PROTOTYPE(void init_atom_stack,(void));
PROTOTYPE(ATOM mark_atom_stack,(void));
PROTOTYPE(void rel_atom_stack,(ATOM));

PROTOTYPE(boolean is_heap_atom,(ATOM));
PROTOTYPE(boolean is_stack_atom,(ATOM));
PROTOTYPE(ATOM get_stack_atom,(ATOM));
PROTOTYPE(boolean norm_atom,(ATOM));
PROTOTYPE(boolean func_atom,(ATOM));

PROTOTYPE(boolean is_number,(ATOM));
PROTOTYPE(boolean is_integer,(ATOM));

PROTOTYPE(ATOM LOOKUP,(string,int,boolean,boolean));
PROTOTYPE(ATOM LOOKATOM,(ATOM,int,boolean));
PROTOTYPE(ATOM GetAtom,(ATOM));
#if QUICK_BUT_DIRTY
IMPORT ATOM ATOMSTOP;
IMPORT ATOM ATOMHTOP;
#define init_atom_stack()	(ATOMSTOP = MAX_ATOM)
#define mark_atom_stack()	(ATOMSTOP)
#define rel_atom_stack(A)	(ATOMSTOP = (A))
#define is_heap_atom(A)		((A) <= ATOMHTOP)
#define is_stack_atom(A)	((A) >= ATOMSTOP)
#define norm_atom(A)		((A) >= NORMATOM)
#define func_atom(A)		((A) >= FUNCNAME)
#endif

#if HIGHER_ORDER
#define var_call(A)		(class(A) == VARCP)
#endif
#if !HIGHER_ORDER
#define var_call(A)		(0)
#endif

/** HASHTAB **/
#define HASHSIZE 0x100 
#define hashcode(C1,C2)  ((((C1) & 0x7f)<<1)|((((C1)?(C2):0)&0x40)>>6))
#define strhash(S)       hashcode(*S,*(S+1))
extern ATOM HASHTAB[ HASHSIZE ];

/*
** TERM
*/
#if BYTEMODE || WORDMODE
#define name(term)       faracc(ATOM,_t_NAME,term)
farexternal(ATOM,_t_NAME, N_OF_TERMS);

#define son(term)        faracc(TERM,_t_VALUE,term)
#define ival(term)       faracc(int,_t_VALUE,term) 
#define val(term)        faracc(TERM,_t_VALUE,term)
#define offset(term)     faracc(int ,_t_VALUE,term)
farexternal(card,_t_VALUE, N_OF_TERMS);

#define NIL_TERM	0
#define MIN_TERM	(NIL_TERM + TERM_UNIT)
#endif

#if POINTERMODE
external(struct _NODE , _nodetab, N_OF_TERMS);

#define name(term)      ((term)->t_name)
#define son(term)	(((term)->t_union).t_son)
#define ival(term)	(((term)->t_union).t_ival)
#define val(term)	(((term)->t_union).t_val)
#define offset(term)	(((term)->t_union).t_offset)

#define NIL_TERM	((TERM)0)
#define MIN_TERM	(&_nodetab[0])
#endif

PROTOTYPE(TERM br,(TERM));
PROTOTYPE(TERM n_brother,(TERM,int ));
#define next_br(T)       ((T)  = br(T))

PROTOTYPE(TERM stackterm,(int ));
PROTOTYPE(TERM heapterm,(int ));
PROTOTYPE(void freeblock,(int ,TERM));

PROTOTYPE(void init_term_stack,(void));
PROTOTYPE(TERM mark_term_stack,(void));
PROTOTYPE(void rel_term_stack,(TERM));

PROTOTYPE(boolean is_heap_term,(TERM));
PROTOTYPE(boolean is_stack_term,(TERM));

PROTOTYPE(boolean is_older_term,(TERM,TERM));

#if QUICK_BUT_DIRTY
#define br(T)		((T) +  TERM_UNIT)
#undef next_br
#define next_br(T)	((T) += TERM_UNIT)
#define n_brother(T,n)	((T) +  (n)*TERM_UNIT)

IMPORT TERM GLOTOP,HEAPTOP;

#define init_term_stack()	(GLOTOP = MAX_TERM)
#define mark_term_stack()	(GLOTOP+0)
#define rel_term_stack(T)	(GLOTOP = T)

#define is_stack_term(T)        (GLOTOP <= (T))
#define is_heap_term(T)	        (HEAPTOP > (T))

#define is_older_term(T1,T2)	((T1) > (T2))
#endif

PROTOTYPE(TERM get_stack_term,(TERM));

PROTOTYPE(TERM mkfreevar,(void));
PROTOTYPE(TERM mkint,(int));
#if REALARITH
PROTOTYPE(TERM mkreal,(REAL));
PROTOTYPE(REAL realval,(TERM));
#endif
#if LONGARITH
PROTOTYPE(TERM mklong,(LONG));
PROTOTYPE(LONG longval,(TERM));
#endif
PROTOTYPE(TERM mkatom,(ATOM));
PROTOTYPE(TERM mkfunc,(ATOM,TERM));
PROTOTYPE(TERM mk2sons,(/* nicht spezifiziert */));
PROTOTYPE(TERM stackvar,(int ));
PROTOTYPE(TERM arg1,(TERM));
PROTOTYPE(TERM arg2,(TERM));
PROTOTYPE(TERM arg,(int,TERM));

PROTOTYPE(void Init_Skeleton,(void));
PROTOTYPE(TERM SKELETON,(TERM,int ,int));
PROTOTYPE(int  Var_Count,(void));


/*
** CLAUSE
*/

#define _head_offset     2
#define nextcl(clause)   (*(CLAUSE FAR *)&val((TERM)(clause)))
#define head(clause)     n_brother((TERM)(clause),_head_offset)
#define body(clause)     n_brother((TERM)(clause),(_head_offset+1))

#define _cl_var(clause)	    (ival(br((TERM)(clause))))

#define in_use(clause)	    (_cl_var(clause) & 0x8000 )
#define set_in_use(clause)  (_cl_var(clause) |= 0x8000 )
#define unset_in_use(clause)(_cl_var(clause) &= 0x7fff )

#define nvars(clause)       (_cl_var(clause) & 0x7fff )
#define setnvars(clause,N)  (_cl_var(clause) = N | in_use(clause))


#define NIL_CLAUSE	(CLAUSE)(NIL_TERM)

IMPORT CLAUSE DUMMYCL;
PROTOTYPE(CLAUSE NewClause,(TERM,TERM,boolean));
PROTOTYPE(void FreeClause,(CLAUSE));

/*
** TRAIL
*/

#if BYTEMODE || WORDMODE
#define boundvar(v)  faracc(TERM,_TRAILTAB,v) 
farexternal(TERM,_TRAILTAB, N_OF_TRAILS);

#define NIL_TRAIL	0
#define MIN_TRAIL	(NIL_TRAIL + TRAIL_UNIT)
#endif
#if POINTERMODE
external( struct _TRAIL, _trailtab, N_OF_TRAILS);
#define boundvar(v)  	((v)->t_term)

#define NIL_TRAIL	((TRAIL)0)
#define MIN_TRAIL	(&_trailtab[0])
#endif

PROTOTYPE(void init_trail_stack,(void));
PROTOTYPE(TRAIL mark_trail_stack,(void));
PROTOTYPE(void rel_trail_stack,(TRAIL));
PROTOTYPE(void trailterm,(TERM));

#if QUICK_BUT_DIRTY
IMPORT TRAIL TRAILEND;
#define init_trail_stack()	(TRAILEND = MIN_TRAIL)
#define mark_trail_stack()	(TRAILEND+0)
#define rel_trail_stack(TT)	do{register TRAIL T = (TT),TE=TRAILEND;\
				while(T<TE){name(boundvar(T))=UNBOUNDT;\
				 inc_trail(T); } TRAILEND = TT; }      \
				 while(0) /* no ; */
#endif
/*
** ENV
*/

#if BYTEMODE || WORDMODE
farexternal(TERM,_e_CALL, N_OF_ENVS);
farexternal(TERM,_e_BASE, N_OF_ENVS);
farexternal(ENV,_e_ENV, N_OF_ENVS);
farexternal(ENV,_e_CHOICE, N_OF_ENVS);
farexternal(TERM,_e_TERM, N_OF_ENVS);
farexternal(ATOM,_e_ATOM, N_OF_ENVS);
farexternal(STRING,_e_STRING, N_OF_ENVS);
farexternal(CLAUSE,_e_CLAUSE, N_OF_ENVS);
farexternal(TERM,_e_BCT, N_OF_ENVS);
farexternal(TRAIL,_e_TRAIL, N_OF_ENVS);
farexternal(card,_e_KIND, N_OF_ENVS);

#define call(e)        faracc(TERM,_e_CALL,e)
#define base(e)        faracc(TERM,_e_BASE,e)
#define env(e)         faracc(ENV,_e_ENV,e)
#define choice(e)      faracc(ENV,_e_CHOICE,e)
#define termtop(e)     faracc(TERM,_e_TERM,e)
#define atomtop(e)     faracc(ATOM,_e_ATOM,e)
#define stringtop(e)   faracc(STRING,_e_STRING,e)
#define rule(e)        faracc(CLAUSE,_e_CLAUSE,e)
#define bct(e)         faracc(TERM,_e_BCT,e)
#define trail(e)       faracc(TRAIL,_e_TRAIL,e)
#define callkind(e)    faracc(card,_e_KIND,e)

#define NIL_ENV		0
#define MIN_ENV		(NIL_ENV + ENV_UNIT)
#endif
#if POINTERMODE
external( struct _ENV , _envtab, N_OF_ENVS);

#define call(e)        ((e)->e_call)
#define base(e)        ((e)->e_base)
#define env(e)         ((e)->e_env)
#define choice(e)      ((e)->e_choice)
#define termtop(e)     ((e)->e_termtop)
#define atomtop(e)     ((e)->e_atom)
#define stringtop(e)   ((e)->e_string)
#define rule(e)        ((e)->e_rule)
#define bct(e)         ((e)->e_bct)
#define trail(e)       ((e)->e_trail)
#define callkind(e)    ((e)->callkind)

#define NIL_ENV		((ENV)0)
#define MIN_ENV		(&_envtab[0])
#endif

PROTOTYPE(ENV newenv,(void));
PROTOTYPE(void init_env_stack,(void));
PROTOTYPE(ENV mark_env_stack,(void));
PROTOTYPE(void rel_env_stack,(ENV));

PROTOTYPE(boolean is_older_env,(ENV,ENV));
PROTOTYPE(ENV get_env,(ENV));

#if QUICK_BUT_DIRTY
IMPORT ENV ENVTOP;
#define init_env_stack()	(ENVTOP = MIN_ENV)
#define mark_env_stack()	(ENVTOP+0)
#define rel_env_stack(E)	(ENVTOP = (E))
#define is_older_env(E1,E2)	((E1) < (E2))
#endif


/*
**
*/

#define deref(T)        deref_(T,BE)
#define deref_(T,B)	(T=DEREF_(T,B))
PROTOTYPE(TERM DEREF_,(TERM,TERM));

#if QUICK_BUT_DIRTY
#undef deref_
#define deref_(T,B)  if(0);else {if(name(T)==SKELT)\
			    T=n_brother(B,offset(T));\
			    while(name(T)==VART)T=val(T);}
#define DEREF_      damit kein Code erzeugt wird
#endif

IMPORT TERM BE;
IMPORT ENV CHOICEPOINT;

PROTOTYPE(boolean UNI,(TERM,TERM));
#if QUICK_BUT_DIRTY
#define UNI(T1,T2)	UNIFY(1,(T1),(T2),BE,BE,MAXDEPTH)
#endif



typedef card	ERRORTYPE;

#define NOERROR              0
#define ABORTE               1
#define ARGE                 2
#define ATOMSPACEE           3
#define BADARITYE            4
#define BADCDDE              5
#define BADCHARE             6
#define BADDOTE              7
#define BADEXPE              8
#define BADKETE              9
#define BADTYPE              10
#define CALLE                11
#define COMMENTE             12
#define DEPTHE               13
#define DIV0E                14
#define EOFE                 15
#define FRAMESPACEE          16
#define IOERROR              17
#define LOCALSPACEE          18
#define NEEDOPE              19
#define NEEDQUOTEE           20
#define NEEDRANDE            21
#define NUMBERSYNE           22
#define NVARSE               23
#define PRECE                24
#define HELPE                25
#define READNESTE            26
#define READSTACKE           27
#define STDFUNCARGE          28
#define SYSPROCE             29
#define TRAILSPACEE          30
#define UNDEFFUNCE           31
#define VARSPACEE            32
#define WIERDCHE             33
#define aSTRINGSPACEE        34
#define FPEE                 35
#define CANTCR               36
#define CANTOP               37 
#define NOTOPEN              38
#define ISTTYE               39
#define TOMANY               40
#define CUROUT               41
#define CURINP               42
#define ONLOUT               43 
#define ONLINP               44

#if SAVE
#define BAD_MAGIC		45
#define BAD_CHKSUM		46
#endif

#endif

#define		WARN_OFF    	0x00
#define		WARN_ON		0x7f
#define 	WARN_OP		0x01
#define 	WARN_RETRACT	0x02
#define		WARN_CONSULT	0x04
#define 	WARN_LISTING	0x08
#define 	WARN_SPY	0x10
#define		WARN_READ	0x20
#define 	WARN_ASSIGN	0x40

IMPORT card WARNFLAG;


/* end of file */
