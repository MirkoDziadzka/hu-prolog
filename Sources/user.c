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
#include "atoms.h"
#include "types.h"
#include "extern.h"
#include "files.h"

#if USER

#define useratom(i)        (LAST_ATOM + i * ATOM_UNIT)

LOCAL boolean DOSORT(delete)
    boolean delete;
{
    TERM T,TT,S;
    TERM Res;
    int c;
    if(!islist(A0, false, false)) 
        ERROR(ARGE);
    Res = mkatom(NIL_0);
    S = A0;
    while(name(S) == CONS_2)
    {   /* insert arg1(S) */
        T = arg1(S);
        S=arg2(S);
        TT = Res; 
        c=1; /* set c to a value != 0 ! */
        while(name(TT)==CONS_2 && (c=compare(arg1(TT),T,MAXDEPTH)) < 0)
            TT = arg2(TT);
        if(delete && c == 0) continue;
        son(TT) = mk2sons(name(T),son(T),name(TT),son(TT));
        name(TT) = CONS_2;
    }
    return UNI(A1,Res);
}

boolean is_a_tty(T)
    TERM T;
{
    file f;
    CHECKATOM(T);
    if((f = OpenFile(phy_name(name(T)),look_mode)) < FIRSTFILE)
    {
	FileError(NOTOPEN);
	return false;
    }
    return ISTTY(f);
}

void Init_User()
{
#if DEBUG
    if(DEBUGFLAG)
        out(2,"Init_User\n");
#endif
    InitUAtom(useratom(0),"sort",NONO,2,EVALP,true);
    InitUAtom(useratom(1),"sort0",NONO,2,EVALP,true);
    InitUAtom(useratom(2),"isatty",NONO,1,EVALP,true);
    InitUAtom(useratom(3),"$linenumber",NONO,1,EVALP,true);
    InitUAtom(useratom(4),"$error_string",NONO,2,EVALP,true);
    InitUAtom(useratom(5),"$getpid",NONO,1,EVALP,true);
}

/*ARGSUSED*/
boolean CallUser(X,ENVP) 
    TERM X;
    ENV ENVP;
{ 
    boolean res= false;
    switch(name(X))
    {
    case useratom(0): res = DOSORT(false);break;
    case useratom(1): res = DOSORT(true);break;
    case useratom(2): res = is_a_tty(A0); break;
    case useratom(3): res = INTRES(A0,LINENUMBER); break;
    case useratom(4): res = UNI(A0,mkatom(error_string(INTVALUE(A1))));
			    break;
    case useratom(5): res = INTRES(A0,getpid()); break;
    default: ws("unknown evaluable predicate\n");
    }
    return res;
}
#endif


/* end of file */
