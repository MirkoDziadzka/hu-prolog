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
#if WINDOWS

#if UNIX
#  include <termio.h>
   typedef struct termio	term_struct;
#   ifndef TCGETATTR
#    ifdef TCGETA
#     define TCGETATTR	TCGETA
#     define TCSETATTR	TCSETA
#    endif
#    ifdef TCGETS
#     define TCGETATTR	TCGETS
#     define TCSETATTR	TCSETS
#    endif
#   endif
   extern int ioctl();

static term_struct old_struct;
static term_struct new_struct;
#endif

static int t_is_init = 0;

/* MS_DOOF defaults */
char EOF_CHAR = 'z' & 0x1f;
char EOL_CHAR = '\r';
char ERASE_CHAR = '\b';

void t_init()
{
    if(t_is_init)
	return;
#if UNIX
    (void)ioctl(0, TCGETATTR , &old_struct);
    (void)memcpy((char *)&new_struct , (char *)&old_struct , 
	   sizeof (term_struct));
    new_struct.c_cc[VMIN] = 1;
    new_struct.c_cc[VTIME] =0;
    new_struct.c_lflag &= ~(ICANON | ECHO);
    new_struct.c_lflag |= ISIG;
    new_struct.c_iflag &= ~(ICRNL);
    new_struct.c_iflag |= INLCR;

    (void)ioctl(0,TCSETATTR,&new_struct);

    if(old_struct.c_cc[VEOF])
	    EOF_CHAR = old_struct.c_cc[VEOF];
    if(old_struct.c_cc[VERASE])
	    ERASE_CHAR = old_struct.c_cc[VERASE];
    /*
    if(old_struct.c_cc[VEOL])
	    EOL_CHAR = old_struct.c_cc[VEOL];
    */

#endif
    t_is_init = 1;
}

void t_exit()
{
    if(! t_is_init)
	return;
#if UNIX
    (void)ioctl(0,TCSETATTR,&old_struct);
#endif
    t_is_init = 0;
}

#endif

