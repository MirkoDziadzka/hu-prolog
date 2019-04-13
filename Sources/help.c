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

#include <string.h>

#include "config.h"
#include "atoms.h"
#include "types.h"
#include "files.h"
#include "extern.h"

#if HELP

#include "paths.h"
#include "help.h"

/************ Interface *************/

#define OK	0
#define ERROR	1
#define EXIT	2

int HELPFILE;

#define h_setpos(p)	(void)lseek(HELPFILE,(long)(p),0)
#define h_getpos()	lseek(HELPFILE,0L,1)
#define h_close()	close(HELPFILE)

int h_open(filename)
    char *filename;
{
    if((HELPFILE = open(filename,0)) < 0)
	return ERROR;
    else
	return OK;
}

int h_getline(buffer,MaxLen)
    char buffer[];
    int MaxLen;
{
    int r;
    int i;
    long pos;
    pos = h_getpos();
    r = read(HELPFILE,buffer,MaxLen);
    for(i = 0 ; ; ++i)
    {
	if(i == r)
	    return ERROR;
	if(buffer[i] == '\n')
	{
	    buffer[i] = '\0';
	    break;
	}
    }
    h_setpos(pos + i + 1);
    return OK;
}

/* #define h_putstring(s)	ws((s)) */
#if EXTENDED_ASCII
#define UMLAUT_a	"\204"
#define UMLAUT_o	"\224"
#define UMLAUT_u	"\201"
#define UMLAUT_A	"\216"
#define UMLAUT_O	"\231"
#define UMLAUT_U	"\232"
#define UMLAUT_s	"\341"
#endif 
#if LATIN1
#define UMLAUT_a	"\344"
#define UMLAUT_o	"\366"
#define UMLAUT_u	"\374"
#define UMLAUT_A	"\304"
#define UMLAUT_O	"\326"
#define UMLAUT_U	"\334"
#define UMLAUT_s	"\337"
#endif
#ifndef UMLAUT_a
#define UMLAUT_a	"ae"
#define UMLAUT_o	"oe"
#define UMLAUT_u	"ue"
#define UMLAUT_A	"AE"
#define UMLAUT_O	"OE"
#define UMLAUT_U	"UE"
#define UMLAUT_s	"ss"
#endif
void h_putstring(s)
    char *s;
{
    while(*s)
    {
	if(*s == '"' )
	{
	    switch(*++s)
	    {
		case '\0': 	return; 
		case 'a':	ws(UMLAUT_a); break;
		case 'o':	ws(UMLAUT_o); break;
		case 'u':	ws(UMLAUT_u); break;
		case 'A':	ws(UMLAUT_A); break;
		case 'O':	ws(UMLAUT_O); break;
		case 'U':	ws(UMLAUT_U); break;
		case 's':	ws(UMLAUT_s); break;
		case '"':	wc('"'); break;
		default:	wc('"');wc(*s); break;
	    }
	}
	else
	    wc(*s);
	++s;
    }
}

#define h_putint(i)	ws(itoa((i)))
#define h_cls()         DOCLS()

int ask_user(buffer,MaxLen)
    char buffer[];
    int MaxLen;
{
    int i = 0;
    GETCHAR();
    while(CH != '\n')
    {
	if(i < MaxLen)
	    buffer[i++] = CH;
	GETCHAR();
    }
    buffer[i] = '\0';
    if(FILEENDED())
	(void)strcpy(buffer,"0");
    return OK;
}

#define h_internal_error()	/*ws(itoa(__LINE__)),*/ ABORT(HELPE)

/************ Interface ends here *************/

#define MAX_LINE_LEN	80
#define MAX_SUBENTRIES	256

LOCAL char buffer[MAX_LINE_LEN+1];
LOCAL char subbuf[MAX_SUBENTRIES][MAX_LINE_LEN+1];

LOCAL long INDEXSTART, TEXTSTART;
LOCAL boolean is_init = false;

LOCAL void open_helpfile()
{
    char *s;
    if((s = getenv(HUP_HELP_ENV)) == (char *)0)
	s = STD_HELP_FILE ;
    if(h_open(s) == ERROR)
	h_internal_error();
    h_setpos(0L);
    for(;;)
    {
	if(h_getline(buffer,MAX_LINE_LEN) == ERROR)
	    h_internal_error();

	if(strcmp(buffer,INDEX_MARK) == 0)
	    break;
    }
    /* INDEX Table found */
    INDEXSTART = h_getpos();
    /* searching TEXT */
    for(;;)
    {
	if(h_getline(buffer,MAX_LINE_LEN) == ERROR )
	    h_internal_error();
	if(strcmp(buffer,TEXT_MARK) == 0)
	    break;
    }
    TEXTSTART = h_getpos();
    is_init = true;
}

LOCAL char lower(ch)
    char ch;
{
    if(ch >= 'A' && ch <= 'Z')
	return ( (ch - 'A') + 'a');
    else
	return ch;
}

LOCAL int match_string(topic,help_entry)
    char *topic;
    char *help_entry;
/* -1 -> Fehler
**  n -> Zahl der nichtbeachteten Stellen in help_entry
*/
{
    for(;;)
    {
	while(*topic == ' ')
	    ++topic;
	while(*help_entry == ' ')
	    ++help_entry;
	/*out(2,"match_string(");out(2,topic);out(2," , ");*/
	/*out(2,help_entry); out(2,")\n");*/
	if(*topic == '\0')
	{
	    int c;
	    for( c= 0; *help_entry != '\0' ; ++help_entry,++c)
		;
	    return c;
	}
	if(lower(*topic) != lower(*help_entry))
	    return -1;
	++topic;
	++help_entry;
    }
    /*NOTREACHED*/
}

#define isdigit(c)    ((c) >= '0' && (c) <= '9')

int scanlong(long_buffer)
    char long_buffer[];
{
    long l = 0;
    char *s = long_buffer;
    while(*s == ' ')
	++s;
    if(!isdigit(*s))
	return -1L;
    while(isdigit(*s))
	l = (l * 10) + (*s++ - '0');
    return l;
}

long lookup(topic)
    char *topic;
{
    long l;
    int i;
    char *s;
    char buf[MAX_LINE_LEN+1];
    int best_match = -1L;
    long adr_of_best_match = -1L;

    h_setpos(INDEXSTART);
    for(;;)
    {
	(void)h_getline(buf,MAX_LINE_LEN);
	if(buf[0] == MARK_TEXT)
	{
	    s = buf+1;
	    l = 0;
	    while(*s >= '0' && *s <= '9')
		l = (l * 10) + (*s++ - '0');
	    i = match_string(topic,s);
	    if(i >= 0 && (i < best_match || best_match == -1L))
	    {
		best_match = i;
		adr_of_best_match = l;
	    }
	    continue;
	}
	if(best_match >= 0)
	    return (adr_of_best_match + TEXTSTART);
	else
	    return -1L;
    }
}

int help(topic_ptr)
    char *topic_ptr;
{
    long pos;
    unsigned int subecnt;
    char topic[MAX_LINE_LEN+1];
    int i;
    int res;
    int lines;

    (void)strncpy(topic,topic_ptr,MAX_LINE_LEN);

    if((pos = lookup(topic)) == -1L)
	return ERROR;

  again:;
    h_cls();
    subecnt = 0;

    h_setpos(pos);

    (void)h_getline(buffer,MAX_LINE_LEN);
    if(buffer[0] != MARK_TOPIC)
	h_internal_error();

    h_putstring("Topic: ");
    h_putstring(buffer+1);
    h_putstring("\n");
    h_putstring(MARK);
    lines = 0;
    for(;;)
    {
	if(h_getline(buffer,MAX_LINE_LEN) == ERROR)
	    goto end;
	switch(buffer[0])
	{
	    case MARK_LINK:
			if((pos = lookup(buffer+1)) == -1L)
			    goto end;
			{
			    h_setpos(pos);
			    (void)h_getline(buffer,MAX_LINE_LEN);
			    if(buffer[0] != MARK_TOPIC)
				h_internal_error();
			    break;
			}
	    case MARK_SUBTOPIC: 	
			if(subecnt < MAX_SUBENTRIES)
			{
			    (void)strcpy(subbuf[subecnt],buffer+1);
			    ++subecnt;
			}
			else
			   return ERROR;
			break;
	    case MARK_TOPIC:
			goto end;
	    case MARK_COMMENT:
			/* do nothing */
			break;
	    default:
			h_putstring(buffer);
			h_putstring("\n");
			++lines;
			break;
	}
    }
    /*NOTREACHED*/
  end:;
    if(lines)
	h_putstring(MARK);

    if(subecnt)
    {
	int ii;
	/*
	qsort((char *)subbuf , 
	      subecnt , 
	      sizeof(subbuf) / MAX_SUBENTRIES , 
	      strcmp
	);
	*/
	for(ii = 0 ; ii < subecnt ; ++ii)
	{
	    h_putint(ii+1);
	    h_putstring(" : ");
	    h_putstring(subbuf[ii]);
	    h_putstring("\n");
	}
	h_putstring(MARK);
    }
  ask:;
    h_putstring("Enter Command or type `help'): ");
    if(ask_user(buffer,MAX_LINE_LEN) == ERROR)
	return ERROR;
    if((i = (int)scanlong(buffer)) >= 0)
    {
	if(i > 0 && i <= subecnt)
	{
	    if((res = help(subbuf[i-1])) == EXIT)
		return EXIT;
	    else if(res == ERROR)
	    {
		h_putstring("\n-- sorry, i can't find this entry --\n");
		goto ask;
	    }
	}
	else if(i == 0)
	    return EXIT;
	else
	    goto ask;
    }
    else if(strcmp(buffer,"") == 0)
	goto ask;
    else if(strcmp(buffer,".") == 0)
	goto again;
    else if(strcmp(buffer,"..") == 0)
	return OK;
    else if((res = help(buffer)) == EXIT)
	return EXIT;
    else if(res == ERROR)
	goto ask;
    goto again;
}

boolean DOHELP(A)
    ATOM A;
{
    boolean result = false;
    TERM oldin, oldout;
    oldin = FNAME(inputfile);
    oldout = FNAME(outputfile);

    if(!is_init)
	open_helpfile();

    inputfile = outputfile = OpenFile(phy_name(STDHELP_0), read_write);
    if(outputfile >= FIRSTFILE)
    {
	if(help((A == NIL_ATOM) ? 
		   ROOT_TOPIC : 
		   tempstring(atomstring(A))
	) == ERROR)
	    result = false;
	else
	    result = true;
	EOF(inputfile) = false;
    }

    inputfile = OpenFile(oldin,read_mode);
    outputfile=OpenFile(oldout,write_mode);
    return result;
}

#endif

/* end of file */
