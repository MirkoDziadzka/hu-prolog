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


#ifndef __MEMORY_H_INCLUDED__
#define __MEMORY_H_INCLUDED__

#ifndef MEMORYSIZE
#define MEMORYSIZE   300 /* KByte */      
#endif

#if MEMORYSIZE==64 /* KByte */
#define N_OF_ATOMS       500  /* MAX. no. of atoms */
#define N_OF_TERMS      4000  /*  "    "   " term */
#define N_OF_TRAILS     1200  /*  "    "   " critical variables */ 
#define N_OF_ENVS        400  /*  "    "   " active goals */
#define N_OF_STRINGS    3500  /*  "    "   " chars in string tab */
#endif

#if MEMORYSIZE==120 /* KByte */
#define N_OF_ATOMS      1000  /* MAX. no. of atoms */
#define N_OF_TERMS     18000  /*  "    "   " term */
#define N_OF_TRAILS     2000  /*  "    "   " critical variables */ 
#define N_OF_ENVS       1000  /*  "    "   " active goals */
#define N_OF_STRINGS    8400  /*  "    "   " chars in string tab */
#endif

#if MEMORYSIZE==300 /* KByte */
#define N_OF_ATOMS      2000  /* MAX. no. of atoms */
#define N_OF_TERMS     32700  /*  "    "   " term */
#define N_OF_TRAILS     4000  /*  "    "   " critical variables */ 
#define N_OF_ENVS       1500  /*  "    "   " active goals */
#define N_OF_STRINGS   25000  /*  "    "   " chars in string tab */
#endif

#if MEMORYSIZE==2000 /* KByte */
#define N_OF_ATOMS     10000  /* MAX. no. of atoms */
#define N_OF_TERMS     48000  /*  "    "   " term */
#define N_OF_TRAILS    10000  /*  "    "   " critical variables */ 
#define N_OF_ENVS      10000  /*  "    "   " active goals */
#define N_OF_STRINGS   32000  /*  "    "   " chars in string tab */
#endif

#if MEMORYSIZE==3000 /* KByte */
#define N_OF_ATOMS     10000  /* MAX. no. of atoms */
#define N_OF_TERMS     65000  /*  "    "   " term */
#define N_OF_TRAILS    20000  /*  "    "   " critical variables */ 
#define N_OF_ENVS      30000  /*  "    "   " active goals */
#define N_OF_STRINGS   65000  /*  "    "   " chars in string tab */
#endif

#if MEMORYSIZE==4000 /* KByte */
#define N_OF_ATOMS     10000  /* MAX. no. of atoms */
#define N_OF_TERMS    (256 * 1024)  /*  "    "   " term */
#define N_OF_TRAILS    20000  /*  "    "   " critical variables */ 
#define N_OF_ENVS      30000  /*  "    "   " active goals */
#define N_OF_STRINGS   (64 * 1024)  /*  "    "   " chars in string tab */
#endif

#if MEMORYSIZE== (1024 * 1024) /* KByte */
#define N_OF_ATOMS    (8 * 1024 * 1024)  /* MAX. no. of atoms */
#define N_OF_TERMS    (64 * 1024 * 1024)  /*  "    "   " term */
#define N_OF_TRAILS   (8 * 1024 * 1024)  /*  "    "   " critical variables */ 
#define N_OF_ENVS     (8 * 1024 * 1024)  /*  "    "   " active goals */
#define N_OF_STRINGS  (8 * 1024 * 1024)  /*  "    "   " chars in string tab */
#endif

#define MAX_STRING      (MIN_STRING + STRING_UNIT * (N_OF_STRINGS -1))
#define MAX_ATOM        (MIN_ATOM   + ATOM_UNIT   * (N_OF_ATOMS-1))
#define MAX_TERM        (MIN_TERM   + TERM_UNIT   * (N_OF_TERMS-1))
#define MAX_ENV         (MIN_ENV    + ENV_UNIT    * (N_OF_ENVS-1))
#define MAX_TRAIL       (MIN_TRAIL  + TRAIL_UNIT  * (N_OF_TRAILS-1))


#endif

/* end of file */
