/**********************************************************************
***********************************************************************
**                                                                   **
**  HU-Prolog         Public Domain Version       Release  2.029     **
**                                                                   **
**  Author(s): 87-89  Christian Horn, Mirko Dziadzka, Matthias Horn  **
**             90-93  Mirko Dziadzka                                 **
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
#include "memory.h"
#include "extern.h"

GLOBAL ATOM  HASHTAB [HASHSIZE];

/* ATOMENTRY ATOMTAB[N_OF_ATOMS]; */
#if POINTERMODE
struct _ATOM _atomtab[N_OF_ATOMS+1];
#endif
#if BYTEMODE || WORDMODE
declare(CLAUSE,_a_CLAUSE,N_OF_ATOMS);
declare(int,_a_ARITY,N_OF_ATOMS);
fardeclare(STRING,_a_STRING,N_OF_ATOMS);
fardeclare(ATOM,_a_NEXTATOM,N_OF_ATOMS);
fardeclare(ATOM,_a_CHAINATOM,N_OF_ATOMS);
fardeclare(card,_a_BIT2,N_OF_ATOMS);
fardeclare(card,_a_BIT1,N_OF_ATOMS);
#endif

/* char STRINGTAB[N_OF_STRINGS]; */
#if BYTEMODE || WORDMODE
fardeclare(char,_STRINGTAB,N_OF_STRINGS);
#endif
#if POINTERMODE
#if !QUICK_BUT_DIRTY
struct _STRING _stringtab[N_OF_STRINGS+1];
#endif
#if QUICK_BUT_DIRTY
char _stringtab[N_OF_STRINGS+1];
#endif
#endif

/* end of file */
