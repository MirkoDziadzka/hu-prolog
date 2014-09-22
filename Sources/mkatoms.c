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


#include <stdio.h>
#include <ctype.h>
#ifdef BSD
#include <strings.h>
#else
#include <string.h>
#endif

extern void exit();
#ifndef __STDC__
/* extern int strlen(); */
#endif
extern int strncmp();

#define MAXL 512        /* bufferlength */
#define DIST  20        

char Linemark[] = "#atom";
int counter = 0;

char linebuffer[MAXL];

int main(argc,argv)
    int argc;
    char *argv[];
{
    register char *strptr;
    register int i;

    while((strptr=fgets(linebuffer,MAXL,stdin))!=NULL)
    {
        i = 0;
        if(strlen(strptr) > MAXL-3)
        {
            fprintf(stderr,"%s:line to long -- abort\n",argv[0]);
            exit(1);
        }
	while(isspace(*strptr))
	    ++strptr;
        if(strncmp(Linemark,strptr,strlen(Linemark)) != 0)
            fputs(strptr,stdout);
        else
        {
            strptr += strlen(Linemark);
            fputs("#define ",stdout);
            while(isspace(*strptr))
                ++strptr;
            while(*strptr > ' ')
            {
                putc(*strptr++,stdout);
                i++;
            }
            while(i++ < DIST)
                putc(' ',stdout);
            fprintf(stdout,"(MIN_ATOM + %3d * ATOM_UNIT)\n",
                               counter++);
        }
    }
    return 0;
}



/* end of file */
