#include <ncurses.h>
#include <iostream>
#include "tile.h"
#include "display.h"
using namespace std;
static char _RESET_COLOR[]="\033[0m";
static char _RED[]="\033[1;31m";
static char _WHITE[]="\033[1;39m";
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
	field[y][x].setc(_WHITE);
	field[y][x].m=false;
}
void Display::statichouse()
{
	for (int y=15;y<20;y++)
		for (int x=15;x<25;x++)
			field[x][y].wall();
	for (int y=16;y<19;y++)
		for (int x=16;x<24;x++)
			field[x][y].floor();
	field[15][17].floor();
}
