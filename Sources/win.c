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

#include "charcl.h"


extern char *t_cls();
extern char *t_gotoxy();
extern int t_lines();
extern int t_columns();

extern int read();
extern int write();

/* 
**  Compilerabhaengige Definitionen
*/

#include "window.h"             /* Schnittstelle zum Nutzer     */

#define PAGING_INFO	" press any key "

#define REGISTER register
#define STATIC static           

typedef unsigned char byte;

/*
**    allgemeine Konstanten fuer die Windowgroessen
*/

static byte LINES      =  24;
static byte COLUMNS    =  80;

#define MAXCHAR  20000
#define MAXWINDOWS  15

extern char EOF_CHAR;
extern char EOL_CHAR;
extern char ERASE_CHAR;

#define SCREENSIZE (LINES * COLUMNS)

typedef unsigned int word;

/*
**    Realisierung der Windows
*/

STATIC char _w_char[MAXCHAR];
STATIC WIN _up[MAXWINDOWS];
STATIC WIN _down[MAXWINDOWS];
STATIC byte _x_pos[MAXWINDOWS];
STATIC byte _y_pos[MAXWINDOWS];
STATIC byte _x_len[MAXWINDOWS];
STATIC byte _y_len[MAXWINDOWS];
STATIC byte _x_abs[MAXWINDOWS];
STATIC byte _y_abs[MAXWINDOWS];
STATIC byte _used[MAXWINDOWS];
STATIC word _w_start[MAXWINDOWS];
STATIC int  _w_mode[MAXWINDOWS];

STATIC WIN firstwindow = NOWINDOW;
STATIC word firstfree = 0;


/* entsprechende Makros */

#define up(w)           (_up[w])
#define down(w)         (_down[w])
#define x_pos(w)        (_x_pos[w])
#define y_pos(w)        (_y_pos[w])
#define x_len(w)        (_x_len[w])
#define y_len(w)        (_y_len[w])
#define x_abs(w)        (_x_abs[w])
#define y_abs(w)        (_y_abs[w])
#define used(w)         (_used[w])
#define w_start(w)      (_w_start[w])
#define w_mode(w)       (_w_mode[w])

#define w_char(w,x,y)   (_w_char[w_start(w)+(x_len(w)+2)*((y))+(x)])
#define s_char(i)       (_w_char[i])

#define x_scr_pos(w,x)  (x_abs(w) + (x) -1)
#define y_scr_pos(w,y)  (y_abs(w) + (y) -1)
#define inwindow(w,x,y) ( x_scr_pos(w,0) <= (x) && \
                          (x) <= x_scr_pos(w,x_len(w)+1) && \
                          y_scr_pos(w,0) <= (y) && \
                          (y) <= y_scr_pos(w,y_len(w)+1) \
                        )
#define iswindow(w)     ((w) >= 0 && (w) < MAXWINDOWS && used(w) )

#define scr_char(x,y)   (_w_char[(COLUMNS)*((y)-1)+(x)-1])

#define setcursor(w)    gotoxy(x_scr_pos(w,x_pos(w)),\
                                 y_scr_pos(w,y_pos(w)))

#define gotoxy(x,y)   {register byte xx=(x),yy=(y);\
                         if(xx  != cur_x_pos || yy != cur_y_pos) \
                              _gotoxy(xx,yy); \
                        }
/*********************************************************
**   Terminalabhaengige Funktionen
*********************************************************/


/* Modus fuer Pufferung der Ausgabe                     */
#define ON 1
#define OFF 2
#define RESET 3
#define FLUSH 4

#define MAXBUF (8*1024)
STATIC byte buffered = 0;
STATIC int bufpos = 0;
STATIC char screen_buf[MAXBUF];

STATIC byte cur_x_pos = 0 , cur_y_pos = 0;

/*    Zeichen Fuer den Rahmen des Fensters              */
/*   		  ol       or      ul      ur      hor     ver    */
#if EXTENDED_ASCII
char t_box[6] = { '\332',  '\277', '\300', '\331', '\304', '\263' };
#endif
#if ! EXTENDED_ASCII 
char t_box[6] = { ' ',     ' ',    ' ',    ' ',    '-',    '|' };
#endif

extern void t_init();
extern void t_exit();


/* STATIC */
int t_rc()              /* lesen eines Zeichens vom Terminal    */
{                       /* ohne Echo und ohne Pufferung         */

#if UNIX
    char ch;
    if(read(0,&ch,1) != 1) return -1;
    else return ch;
#endif
#if MS_DOS
    extern char getch(); /* from the MSC-LIB */
    return getch();
#endif
}

STATIC void t_buffer(mode)
REGISTER byte mode;
{
    if(mode == ON) 
    {
        buffered++ ; 
        return;
    }
    else if(mode == FLUSH || (mode == OFF && --buffered == 0))
    {
        if(bufpos) 
	    (void)write(1,screen_buf,(unsigned int)bufpos);
        bufpos = 0;
    }
    else if(mode == RESET) bufpos = 0;
}

/*ARGSUSED*/
STATIC void t_wc(c)
char c;
{
    if(!is_no_char(c)) cur_x_pos++;
    if(buffered)
    {
        screen_buf[bufpos++] = c ;
        if(bufpos >= MAXBUF)
        {
            (void)write(1,screen_buf,(unsigned int)bufpos);
            bufpos = 0;
        }
    }
    else (void)write(1,&c,1);
}

STATIC void t_ws(s)
REGISTER char *s;
{
    while(*s) t_wc(*s++);

}

STATIC void _gotoxy(S,Z)
REGISTER byte S,Z;
{
    t_ws(t_gotoxy((int)S,(int)Z));
    cur_x_pos = S; 
    cur_y_pos = Z;
}

STATIC void _cls()
{
    t_ws(t_cls());
    cur_x_pos = 1 ; 
    cur_y_pos =1;
}

STATIC void _beep()
{
    (void)write(1,"\007",1);
}

/*  Routinen zur Windowverwaltung                       */

void Init_Windows()
{
    WIN w;
    REGISTER byte x,y;

    extern int get_num_env_val();

    LINES = (byte)t_lines();
    COLUMNS = (byte)t_columns();

    t_init();
    _cls();

    for(x=1;x<=COLUMNS;++x)
        for(y=1;y<=LINES;++y)
        {
            scr_char(x,y) = ' ';
        }
    for(w = 0; w < MAXWINDOWS ; ++w)
        used(w) = 0;

    firstfree = SCREENSIZE;

    (void)w_create(1,1,(int)COLUMNS,(int)LINES,(char *)0,0);

}

void w_exit()
{
    _gotoxy(1,LINES+1);
    t_exit();
}

STATIC WIN newwindow(size)
word size;
{
    WIN w;
    for(w = 0 ; w < MAXWINDOWS; ++w)
        if(!used(w)) break;
    if(w >= MAXWINDOWS) return NOWINDOW;
    if(firstfree + size >= MAXCHAR) return NOWINDOW;
    used(w) = 1;
    w_start(w) = firstfree;
    firstfree += size;
    return w;
}

STATIC void freewindow(w)
WIN w;
{
    REGISTER WIN ww;
    REGISTER word i;
    word size;
    if(!iswindow(w)) return;
    used(w) = 0;
    i = w_start(w);
    size = (x_len(w) + 2) * (y_len(w) +2);
    for(ww = 0 ; ww < MAXWINDOWS ; ++ww)
        if(used(ww) && w_start(ww) > w_start(w))
            w_start(ww) -= size;
    firstfree -= size;
    for(i = w_start(w) ; i < firstfree ; ++i)
    {
        s_char(i)  = s_char(i+size); 
    }
}

STATIC void w_charout(w,x,y)
REGISTER WIN w;
REGISTER byte x,y;
{
    REGISTER byte xp,yp;
    if((xp = x_scr_pos(w,x)) < 1 || xp > COLUMNS) return ;
    if((yp = y_scr_pos(w,y)) < 1 || yp > LINES) return ;
    if(w_char(w,x,y) == scr_char(xp,yp) ) 
	return;
    gotoxy(xp,yp);
    t_wc(w_char(w,x,y));
    scr_char(xp,yp) = w_char(w,x,y);
}

STATIC void w_out(w)
REGISTER WIN w;
{
    REGISTER byte x,y;
    t_buffer(ON);
    for(y = 0 ; y <= y_len(w) +1 ; ++y)
        for(x = 0 ; x <= x_len(w)+1 ; ++x)
        {
            /* inline code for: w_charout(w,x,y) */
            REGISTER byte xp,yp;
            if((xp = x_scr_pos(w,x)) < 1 || xp > COLUMNS) goto ok ;
            if((yp = y_scr_pos(w,y)) < 1 || yp > LINES) goto ok ;
            if(w_char(w,x,y) == scr_char(xp,yp) ) 
		goto ok;
            gotoxy(xp,yp);
            t_wc(w_char(w,x,y));
            scr_char(xp,yp) = w_char(w,x,y);
ok:
            ;
        }
    t_buffer(OFF);
}

WIN w_create(x,y,xl,yl,s,mode)
int x,y,xl,yl;
char *s;
int mode;
{
    WIN w;
    byte i;

    if(x<1 || x > COLUMNS || y < 1  || y > LINES ||
        xl < 2 || (x+xl-1) > COLUMNS || yl < 2 || (y+yl-1) > LINES)
        return NOWINDOW;
    if((w = newwindow((word)((xl+2)*(yl+2)))) == NOWINDOW) 
        return NOWINDOW;
    x_pos(w) = y_pos(w) = 1;
    x_abs(w) = (byte)x; 
    y_abs(w) = (byte)y;
    x_len(w) = (byte)xl; 
    y_len(w) = (byte)yl;

    w_mode(w) = mode ;

    for(x = 0 ; x <= (x_len(w) +1) ; ++x)
        for(y = 0 ; y <= (y_len(w) +1) ; ++y)
        { 
            w_char(w,x,y) = ' '; 
        }
    for(x = 1; x <= x_len(w) ; ++x)
        w_char(w,x,0) = w_char(w,x,y_len(w)+1) = t_box[4];
    for(y = 1; y <= y_len(w) ; ++y)
        w_char(w,0,y) = w_char(w,x_len(w)+1,y) = t_box[5];
    w_char(w,0,0) = t_box[0]; 
    w_char(w,0,y_len(w)+1) = t_box[2];
    w_char(w,x_len(w)+1,0) = t_box[1];
    w_char(w,x_len(w)+1,y_len(w)+1) = t_box[3];

  
    if(s && (i= (byte)strlen(s)) < (x_len(w)-1))
    {
        i = ((x_len(w) -i +1) /2)+1;
        while(*s)
        {
            w_char(w,i,0) = *s;
            s++;
            i++;
        }
    }

    /* window ist jetzt fertig */
    if(firstwindow != NOWINDOW)
        up(firstwindow) = w;
    down(w) = firstwindow;
    up(w) = NOWINDOW;
    firstwindow = w;
    /* window auf den Bildschirm geben */
    t_buffer(ON);
    w_out(w);
    setcursor(w);
    t_buffer(OFF);
    return w;

}

STATIC void page_window_info(w)
    WIN w;
{
    t_buffer(ON);
    w_up(w);
    if(x_len(w) >= (byte)strlen(PAGING_INFO)+2 &&
       y_scr_pos(w,y_len(w)+1) <= LINES)
    {
	char *s;
	byte x,y;

	y = y_scr_pos(w,y_len(w)+1);
	x = x_scr_pos(w,(x_len(w) - 
			    (byte)strlen(PAGING_INFO)) / 2);
	for( s = PAGING_INFO ; *s ; ++s, ++x)
	{
	    gotoxy(x,y);
	    t_wc(*s);
	    scr_char(x,y) = *s;
	}
	t_buffer(FLUSH);
    }
    else
    {
	t_buffer(FLUSH);
	_beep();
    }
    (void)t_rc();
    t_buffer(OFF);
}

STATIC void scroll_window(w)
REGISTER WIN w;
{
    REGISTER byte i,j;
    t_buffer(ON);
    for(i = 1; i <= x_len(w); ++i)
    {
        for(j = 1;j < y_len(w) ; ++j)
        {
            char cc;
            cc = w_char(w,i,j+1);
            w_char(w,i,j) = cc;
        }
        w_char(w,i,y_len(w)) = ' ';
    }
    w_out(w);
    setcursor(w);
    t_buffer(OFF);
}

void w_remove(w)
WIN w;
{
    REGISTER byte x,y;
    REGISTER WIN ww;
    if(!iswindow(w) || w == STDWIN) return;

    t_buffer(ON);

    if((w_mode(w) & PAGING) == PAGING && 
	       (x_pos(w) != 1 || y_pos(w) != 1))
    {
	page_window_info(w);
    }

    /* window w ausketten */
    if(w == firstwindow)
    {
        firstwindow = down(w);
        if(firstwindow != NOWINDOW) up(firstwindow) = NOWINDOW;
    }
    else
    {
        ww = up(w);
        down(ww) = down(w);
        if(down(ww) != NOWINDOW) up(down(ww)) = ww;
    }
    /* jetzt w mit den Hintergrundzeichen fuellen */
    for(x = 0; x <= x_len(w) +1 ; ++x)
        for(y = 0; y <= y_len(w) +1 ; ++y)
        {
            REGISTER byte xa,ya;
            w_char(w,x,y) = ' ';
            xa = x_scr_pos(w,x);
            ya = y_scr_pos(w,y);
            ww = firstwindow;
            while(ww != NOWINDOW)
            {
                if(inwindow(ww,xa,ya))break;
                ww = down(ww);
            }
            if(ww != NOWINDOW)
            {
                xa = (byte)((int)x_abs(w)-(int)x_abs(ww)+x);
                ya = (byte)((int)y_abs(w)-(int)y_abs(ww)+y);
                w_char(w,x,y) = w_char(ww,xa,ya);

            }
        }
    w_out(w);
    freewindow(w);
    if(firstwindow != NOWINDOW)
        setcursor(firstwindow);
    
    t_buffer(OFF);
}

STATIC char *space = " ";

void w_puts(w,s)
REGISTER WIN w;
REGISTER char *s;
{
    if(!iswindow(w)) return;
    t_buffer(ON);
    w_up(w);
    while(*s)
    {
        switch(*s)
        {
        default:  
	    if(is_no_char(*s))
		break;
            w_char(w,x_pos(w),y_pos(w)) = *s;
            w_charout(w,x_pos(w),y_pos(w));
            if(x_pos(w)++ < x_len(w)) break;
        case '\n': 
            x_pos(w) = 1;
            if(y_pos(w) < y_len(w)) 
		++y_pos(w);
            else if((w_mode(w) & PAGING) == PAGING) 
		w_cls(w);
	    else 
		scroll_window(w);
            break;
        case '\b': 
            if(x_pos(w) > 1) x_pos(w)--;
            break;
        case '\t': 
            do {
                w_puts(w,space);
            } while((x_pos(w) & 7) != 1);
            break;
        }
        s++;
    }
    setcursor(w);
    t_buffer(OFF);
}

void w_up(w)
WIN w;
{
    WIN ww;
    if(!iswindow(w) || w == firstwindow) return;
    /* w ausketten */
    ww=up(w); /* != NOWINDOW i hope */
    down(ww) = down(w);
    if(down(ww) != NOWINDOW) up(down(ww)) = ww;
    /* w neu einketten */
    down(w) = firstwindow;
    up(firstwindow) = w; /* firstwindow != NOWINDOW */
    up(w) = NOWINDOW;
    firstwindow = w;
    t_buffer(ON);
    w_out(w);
    setcursor(w);
    t_buffer(OFF);
}


int w_gets(w,buf,len)
WIN w;
char buf[];
int len;
{
    int l;
    if(!iswindow(w)) return 0;
    w_up(w);
    if((l = (int)x_len(w) - (int)x_pos(w)  +1) < len)  
	len = l;
    setcursor(w);
    t_buffer(FLUSH);
    l=0;
    while(1)
    {
        int ch;
        STATIC char ss[2] = "c";
        ch = t_rc();
        if(ch == -1)break;
        else if(ch == EOF_CHAR) break;
        else if(ch == ERASE_CHAR && l > 0) 
	{ 
            w_puts(w,"\b \b"); 
            --l;
        }
        else if(!is_no_char(ch) || ch == EOL_CHAR)
        {
            buf[l] = (char)(ch == EOL_CHAR ? '\n' : ch);
            ss[0] = buf[l++];
            w_puts(w,ss);
        }
        setcursor(w);
        t_buffer(FLUSH);
        if( l >= len || ch == EOL_CHAR) break;
    }
    return l;
}

void w_gotoxy(w,x,y)
WIN w;
int x,y;
{
    if(!iswindow(w)) return;
    t_buffer(ON);
    if(x < 1) x = 1;
    if(y < 1) y = 1;
    if(x > x_len(w)) x = x_len(w);
    if(y > y_len(w)) y = y_len(w);
    w_up(w);
    x_pos(w) = (byte)x;
    y_pos(w) = (byte)y;
    setcursor(w);
    t_buffer(OFF);
}

int w_get_x_pos( w )
WIN w;
{
    if(!iswindow(w)) return -1;
    return x_pos(w);
}

int w_get_y_pos( w )
WIN w;
{
    if(!iswindow(w)) return -1;
    return y_pos(w);
}

void w_cls(w)
WIN w;
{
    byte x,y;
    if(!iswindow(w)) return;
    t_buffer(ON);

    if((w_mode(w) & PAGING) == PAGING)
	page_window_info(w);

    for(x = 1 ; x <= x_len(w) ; ++x)
        for(y = 1 ; y <= y_len(w) ; ++y)
        {
            w_char(w,x,y) = ' ';
        }
    x_pos(w) = y_pos(w) = 1;
    w_up(w);
    w_out(w);
    t_buffer(OFF);
}

void show_window(w)
    WIN w;
{
    if(!iswindow(w)) return;

    if((w_mode(w) & PAGING) == PAGING)
	w_cls(w);
}

void w_scr_refresh()
{
    byte x,y;

    t_buffer(ON);
    _cls();
    for(y = 1 ; y <= LINES ; ++y)
        for(x  = 1 ; x <= COLUMNS ; ++x)
	{
	    gotoxy(x,y);
	    t_wc(scr_char(x,y));
	}
    if(firstwindow != NOWINDOW)
    {
        w_out(firstwindow);
        setcursor(firstwindow);
    }
    t_buffer(OFF);
}

#endif


/* end of file */
