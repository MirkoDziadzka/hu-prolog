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



/*
**  includefile "window.h"
*/

#define WIN        int
#define STDWIN     ((int)0)
#define NOWINDOW   ((int)-1)

#define NORMAL     ((int)0x00)

#define REVERSE    ((int)0x01)
#define UNDERLINE  ((int)0x02)
#define BLINK      ((int)0x04)
#define BOLD       ((int)0x08)
#define PAGING     ((int)0x10)

extern WIN w_create(/* int x,y,xl,yl ; char *name ; int attr */);
extern void w_remove(/* WIN w */);
extern void w_up(/* WIN w */);
extern void w_puts(/* WIN w ; char *str */);
extern int  w_gets(/* WIN w ; char *buf ; int maxlen */);
extern void w_cls(/* WIN w */);
extern void w_gotoxy(/* WIN w ; int xpos,ypos */);
extern void w_scr_refresh();
extern void w_exit();
extern int t_rc();
extern void Init_Windows();
extern void show_window(/*WIN*/);


/* end of file */
