/**********************************************************************
***********************************************************************
**                                                                   **
**  HU-Prolog         Public Domain Version       Release  2.022     **
**                                                                   **
**  Author(s): 87-89  Christian Horn, Mirko Dziadzka, Matthias Horn  **
**             90-92  Mirko Dziadzka                                 **
**                    Humboldt-Universitaet zu Berlin, FB Informatik **
**                    email: dziadzka@informatik.hu-berlin.de        **
**                                                                   **
**  HU-Prolog 2.022 can be copied, modified and distributed freely   **
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
#include <curses.h>
#endif

static int t_is_init = 0;
static int curses_is_init = 0;

void t_init()
{
    if(t_is_init)
	return;
#if UNIX
    if(!curses_is_init)
    {
	initscr();
	curses_is_init = 1;
	cbreak(); noecho(); 
	savetty();
    }
    resetty();
#endif
    t_is_init = 1;
}

void t_exit()
{
    if(! t_is_init)
	return;
#if UNIX
    endwin();
#endif
    t_is_init = 0;
}

#endif

