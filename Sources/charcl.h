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


#ifndef __CHARCL_H_INCLUDED__
#define __CHARCL_H_INCLUDED__	1

/* 
characters are classified as follows:
small letters  a..z:      SC 
large letters  A..Z, _:   BC 
digits 0..9:              DC
spaces                    C0
atomic characters         OC
special characters ( ) ' " [ ] { }  |  , 
are coded by itself
and all other characters are coded as  0
*/

#define SC 1
#define BC 2
#define DC 3
#define OC 4
#define C0 5 

extern unsigned char _CHARCLASS[256];

#define charclass(ch)	(_CHARCLASS[(unsigned char)(ch)])

#define is_digit(CH)     (charclass(CH) == DC)
#define is_space(CH)     (charclass(CH) == C0 || \
	 		  charclass(CH) == 0)
#define is_s_letter(CH)  (charclass(CH) == SC)
#define is_c_letter(CH)  (charclass(CH) == BC)

#define is_no_char(CH)   (charclass(CH) == 0)
#endif

/* end of file */
