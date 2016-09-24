#include <ncurses.h>
#include <iostream>
#include "tile.h"
#include "display.h"
using namespace std;
static char _RESET_COLOR[]="\033[0m";
static char _L_BLUE[]="\033[1;34m";
Tile field[64][32];
int px,py;
void Display::draw()
{
	cout<<endl;
	for (int y=0;y<32;y++)
	{
		for (int x=0;x<64;x++)
			cout<<field[x][y].retc()<<field[x][y].ret();
		cout<<_RESET_COLOR<<endl<<'\r';
	}
	
}
void Display::placeplayer(int x,int y)
{
	field[y][x].set('@');
	field[y][x].setc(_L_BLUE);
	field[y][x].m=false;
}
void Display::statichouse()
{
	for (int y=15;y<20;y++)
		for (int x=15;x<25;x++)
			field[x][y].wall(x,y);
	for (int y=16;y<19;y++)
		for (int x=16;x<24;x++)
			field[x][y].floor(x,y);
	field[15][17].floor(15,17);
}
