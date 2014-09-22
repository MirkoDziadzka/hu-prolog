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

/*
** (C) 1992	Mirko Dziadzka (dziadzka@informatik.hu-berlin.de)
**
** libPrivat(add_string_to_argv)
**
** Fri Aug 14 14:23:14 MESZ 1992
*/

/*
** envargs ist ein string von Optionen, welche vor argv rangehaengt
** werden sollen. Das Resultat ist dasselbe, als wenn dieser 
** String beim Aufruf des Programms gleich hinter dem Programmnamen
** angegeben wuerde.
**
** Bugs: Die Behandlung von space ist nicht ganz klar.
**       "-o 'file name'" muss nicht so wie in der shell
**       funktionieren.
** 
*/

#include <stdio.h>
#include <ctype.h>

static char  ** new_argv 	= NULL;
static int      new_argv_size 	= 0;
static int      new_argc	= 0;

static char   * progname 	= NULL;

static void 
#ifdef __STDC__
append_string(char *string)
#else
append_string(string)
    char *string;
#endif
{
    int new_size;

    if( new_argv_size == 0 )
	new_size = 8;
    else if (new_argv_size <= new_argc )
	new_size = 2 * new_argv_size;
    else
	new_size = 0;
    
    if ( new_size )
    {
	/* realloc(NULL , ... ) funktioniert nicht immer so, wie
	   es soll
	*/
	if(new_argv == NULL)
	    new_argv = (char **) malloc( new_size * sizeof( char *));
	else
	    new_argv = (char **) realloc((void *)new_argv , 
				       new_size * sizeof( char *));
	
	if( new_argv == NULL )
	{
	    perror( progname );
	    exit(1);
	}

	new_argv_size =  new_size;
    }


    new_argv[new_argc++] = string;

}

static char * 
#ifdef __STDC__
str_cpy( char * string , int len )
#else
str_cpy( string , len )
    char *string;
    int len;
#endif
{
    char * result;
    int i;
    result = (char *)malloc(len+1);

    if( result == NULL )
    {
	perror(progname);
	exit(1);
    }

    for(i = 0 ; i < len ; ++i)
	result[i] = string[i];

    return result;
}

static void 
#ifdef __STDC__
scan_and_append( char * envarg )
#else
scan_and_append( envarg )
    char *envarg;
#endif
{
    char *last_string;

    if(envarg == NULL)
	return;
    
    for(;;)
    {
	if(*envarg == '\0')
	    break;
	if(isspace(*envarg))
	{
	    ++envarg;
	    continue;
	}
	last_string = envarg;
	while(*envarg && !isspace(*envarg))
	    ++envarg;
	append_string(str_cpy(last_string,envarg-last_string));
    }
}

void 
#ifdef __STDC__
add_string_to_argv(int *argc, char ***argv, char * envargs)
#else
add_string_to_argv(argc, argv, envargs)
    int *argc;
    char ***argv;
    char *envargs;
#endif
{
    int i;

    progname = **argv;

    append_string((*argv)[0]);

    scan_and_append( envargs );

    for( i = 1 ; i < (*argc) ; ++i )
	append_string((*argv)[i]);
    
    append_string(NULL);

    *argc = new_argc -1; /* don't count NULL entry */
    *argv  = new_argv;

}
