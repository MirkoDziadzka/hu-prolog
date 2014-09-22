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
#include "memory.h"
#include "extern.h"


GLOBAL void print_copyright()
{
    ws(version);
    ws("Author(s): 87-89  Christian Horn, Mirko Dziadzka, Matthias Horn\n");
    ws("           90-93  Mirko Dziadzka (dziadzka@ai-lab.fh-furtwangen.de)\n");
}

/**************************************************/
/*               date & time                      */
/**************************************************/


#include <time.h>

LOCAL long LTIME;
LOCAL struct tm *TIMEREC;
IMPORT struct tm *localtime();

#if ! BSD
IMPORT long time();
#endif

GLOBAL boolean DOTIME(A)
    ATOM A;
{
    (void)time(&LTIME);
    TIMEREC=localtime(&LTIME);
    switch(A)
    {
    case TIME_3:
        return  INTRES(A0,TIMEREC->tm_hour) && 
                INTRES(A1,TIMEREC->tm_min) && 
                INTRES(A2,TIMEREC->tm_sec);
    case DATE_3:
        return  INTRES(A0,1900 + TIMEREC->tm_year) &&
                INTRES(A1,TIMEREC->tm_mon + 1) &&
                INTRES(A2,TIMEREC->tm_mday);
    case WEEKDAY_1:
        return INTRES(A0,(TIMEREC->tm_wday?TIMEREC->tm_wday:7));
    }
    SYSTEMERROR("misc.c/DOTIME");
    /*NOTREACHED*/
    return false;
}

GLOBAL boolean DOTIMER()
{ 
    static long STARTTIME,CURRTIME;
    CURRTIME = TIMER();
    if (name(A0)==INTT)
    { 
        STARTTIME= CURRTIME-(long)ival(A0); 
        return true; 
    }
    else
#if LONGARITH
        if (name(A0)==LONGT)
        { 
            STARTTIME= CURRTIME-longval(A0); 
            return true; 
        }
        else return LONGRES(A0,CURRTIME-STARTTIME);
#endif
#if ! LONGARITH
    return INTRES(A0,(int)(CURRTIME-STARTTIME));
#endif
}

GLOBAL boolean islist(T, char_list, no_deref)
    register TERM T;
    boolean char_list;
    boolean no_deref;
{
    card counter = 0;
    if(!no_deref)
	deref(T);
    if(char_list)
    {
	register TERM TT;
	while(name(T) == STRING_CONS)
	{
	    TT= son(T);
	    if(!no_deref)
		deref(TT);
	    if(name(TT) != INTT) return false;
	    if(ival(TT) < 32 || ival(TT) > 127) return false;
	    if(!no_deref)
		T = arg2(T);
	    else
		T = br(son(T));
	    if(++counter >= N_OF_TERMS) return false; /* zyklic term */
	}
	return (name(T) == STRING_NIL);
    }
    else
    {
	while(name(T) == CONS_2)
	{
	    if(!no_deref)
		T = arg2(T);
	    else
		T = br(son(T));
	    if(++counter >= N_OF_TERMS) return false; /* zyklic term */
	}
	return (name(T) == NIL_0);
    }
}

/* end of file */
