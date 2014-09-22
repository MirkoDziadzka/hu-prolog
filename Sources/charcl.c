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
#include "charcl.h"

/* 
characters are classified as follows:
small letters  a..z, $:   		SC 
large letters  A..Z, _:   		BC 
digits 0..9:              		DC
spaces                    		C0
atomic characters         		OC
special characters ( ) ' " [ ] { }  |  , 
are coded by itself
and all other characters are coded as  0
*/


GLOBAL unsigned char _CHARCLASS[256] =
{
/*      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
/*--------------------------------------------------------------------*/
/*0*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*1*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*2*/   C0,'!','"', OC, SC, OC, OC,'\'','(',')',OC, OC,',', OC, OC, OC,
/*3*/   DC, DC, DC, DC, DC, DC, DC, DC, DC, DC, OC, OC, OC, OC, OC, OC,
/*4*/   OC, BC, BC, BC, BC, BC, BC, BC, BC, BC, BC, BC, BC, BC, BC, BC,
/*5*/   BC, BC, BC, BC, BC, BC, BC, BC, BC, BC, BC,'[', OC,']', OC, BC,
/*6*/   OC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC,
/*7*/   SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC,'{','|','}', OC,  0,
#if LATIN1
/*8*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*9*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*A*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*B*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*C*/    0,  0,  0,  0, BC,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*D*/    0,  0,  0,  0,  0,  0, BC,  0,  0,  0,  0,  0, BC,  0,  0, SC,
/*E*/    0,  0,  0,  0, SC,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*F*/    0,  0,  0,  0,  0,  0, SC,  0,  0,  0,  0,  0, SC,  0,  0, OC,
#endif
#if EXTENDED_ASCII
/*8*/   BC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, SC, BC, BC,
/*9*/   BC, SC, BC, SC, SC, SC, SC, SC, SC, BC, BC, SC, OC, OC, OC, OC,
/*A*/   SC, SC, SC, SC, SC, BC, SC, SC, OC, OC, OC, OC, OC, OC, OC, OC,
/*B*/   OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC,
/*C*/   OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC,
/*D*/   OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC,
/*E*/   SC, SC, BC, SC, BC, SC, SC, SC, BC, BC, BC, SC, OC, OC, OC, OC,
/*F*/   OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC, OC,  0 
#endif
#if !(EXTENDED_ASCII || LATIN1 )
/*8*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*9*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*A*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*B*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*C*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*D*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*E*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
/*F*/    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 
#endif
};



/* end of file */
