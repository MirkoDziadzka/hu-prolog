/*
** emulate termios whith termio
*/

#  include <termio.h>

#define termios	termio

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

#define tcgetattr( FD , AREA )	ioctl( FD , TCGETATTR , AREA )
/* ignore mode */
#define tcsetattr( FD , MODE , AREA )	ioctl( FD , TCSETATTR , AREA)

