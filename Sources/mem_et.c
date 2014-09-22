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

/* TRAILENTRY TRAILTAB[N_OF_TRAILS] */
#if POINTERMODE
struct _TRAIL   _trailtab[N_OF_TRAILS+1];
#endif
#if BYTEMODE || WORDMODE
fardeclare(TERM,_TRAILTAB,N_OF_TRAILS);
#endif

/* ENVENTRY ENVTAB[N_OF_ENVS]; */
#if POINTERMODE
struct _ENV _envtab[N_OF_ENVS+1];
#endif
#if BYTEMODE || WORDMODE
fardeclare(TERM,   _e_CALL,         N_OF_ENVS);
fardeclare(TERM,   _e_BASE,         N_OF_ENVS);
fardeclare(ENV,    _e_ENV,          N_OF_ENVS);
fardeclare(ENV,    _e_CHOICE,       N_OF_ENVS);
fardeclare(ATOM,   _e_ATOM,         N_OF_ENVS);
fardeclare(TERM,   _e_TERM,         N_OF_ENVS);
fardeclare(STRING, _e_STRING,       N_OF_ENVS);
fardeclare(CLAUSE, _e_CLAUSE,       N_OF_ENVS);
fardeclare(TRAIL,  _e_TRAIL,        N_OF_ENVS);
fardeclare(card,   _e_KIND,         N_OF_ENVS);
fardeclare(TERM,   _e_BCT,          N_OF_ENVS);
#endif

/* end of file */
