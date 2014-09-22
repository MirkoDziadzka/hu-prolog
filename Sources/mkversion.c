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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef RELEASE
#define RELEASE 0
#endif
#ifndef PATCHLEVEL
#define PATCHLEVEL 0
#endif
#ifndef MAGIC_NUMBER
#define MAGIC_NUMBER	NOW_L
#endif


time_t NOW_T;
long   NOW_L;
struct tm *LOCAL;

char *header[] = {
"/**********************************************************************",
"***********************************************************************",
"**                                                                   **",
"**  HU-Prolog         Public Domain Version       Release  %1d.%03d     **",
"**                                                                   **",
"**  Author(s): 87-89  Christian Horn, Mirko Dziadzka, Matthias Horn  **",
"**             90-92  Mirko Dziadzka                                 **",
"**                    Humboldt-Universitaet zu Berlin, FB Informatik **",
"**                    email: dziadzka@informatik.hu-berlin.de        **",
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

    NOW_T = time(&NOW_L); /* aktuelle Zeit in Sekunden */
    LOCAL = localtime(&NOW_T);

    

    for(s = header ; *s != NULL ; ++s)
    {
	printf(*s,RELEASE,PATCHLEVEL);
	printf("\n");
    }
    printf("char *version =\n");
    printf("\"HU-Prolog (Public Domain Version)  Release %1d.%03d\\\n",
	    RELEASE,PATCHLEVEL);
    printf(" (last change: ");
    printf("%02d.%02d.%04d ",LOCAL->tm_mday, LOCAL->tm_mon + 1, 1900 + LOCAL->tm_year);
    printf("%02d:%02d",LOCAL->tm_hour, LOCAL->tm_min);
    printf(")\\n\";\n");
    printf("\nlong MAGIC_NUMBER = %ld;\n",(long)MAGIC_NUMBER);
    printf("\n/* end of file */\n");
    exit(0);
}

/* end of file */
