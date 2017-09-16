#include <termios.h>
#include <stdio.h>
#ifndef M_DISPLAY
#define M_DISPLAY
void clear_screen()
{
	printf("\e[2J");
}
void move_cursor(int x,int y)
{
	printf("\e[%d;%dH",y+1,x+1);
}
void set_cursor_visibility(int visible)
{
	if (visible)
		printf("\e[?25h");
	else
		printf("\e[?25l");
}
void set_terminal_canon(int canon)
{
	static struct termios old_term,new_term;
	if (!canon) {
		if (!old_term.c_lflag)
			tcgetattr(0,&old_term);
		new_term=old_term;
		new_term.c_lflag&=(~ICANON&~ECHO);
		tcsetattr(0,TCSANOW,&new_term);
	} else {
		tcsetattr(0,TCSANOW,&old_term);
	}
}
#endif
