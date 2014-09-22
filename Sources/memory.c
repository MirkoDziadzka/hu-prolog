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
#include "memory.h"
#include "extern.h"

/*
** Deklaration der globalen Felder
**
** Turbo C unter MS-DOS kann nur 64 K globale Daten pro Objektfile
** verwalten. Deswegen ist dieses File in mehrere kleinere aufgeteilt
** worden.
*/

#if TURBOC
# include "TurboC needs the following 4 files insted of memory.c"
#endif

#include "mem_t1.c"
#include "mem_t2.c"
#include "mem_as.c"
#include "mem_et.c"


/* end of file */
