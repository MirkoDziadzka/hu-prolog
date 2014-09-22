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

#include <stdio.h>
#include <stdlib.h>

char *header[] = {
"/**********************************************************************",
"***********************************************************************",
"**                                                                   **",
"**  HU-Prolog         Public Domain Version       Release  %1d.%03d     **",
"**                                                                   **",
"**  Author(s): 87-89  Christian Horn, Mirko Dziadzka, Matthias Horn  **",
"**             90-95  Mirko Dziadzka                                 **",
"**                    Fachhoschule Furtwangen, Allgemeine Informatik **",
"**                    email: dziadzka@ai-lab.fh-furtwangen.de        **",
"**                                                                   **",
"**  HU-Prolog %1d.%03d can be copied, modified and distributed freely   **",
"**  for any non-commercial purposes under the conditions that all    **",
"**  files include this header and that the message                   **",
"**  \"HU-Prolog public domain version\" and the names of the authors   **",
"**  will be displayed, when HU-Prolog is called.                     **",
"**                                                                   **",
"***********************************************************************",
"**********************************************************************/",
NULL
};

void main()
{
    char **s;

    for(s = header ; *s != NULL ; ++s)
    {
	printf(*s,RELEASE,PATCHLEVEL);
	printf("\n");
    }
    exit(0);
}

/* end of file */
