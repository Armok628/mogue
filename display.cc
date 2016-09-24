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
int Display::house(int x,int y,int w,int h,char d)
{
	for (int i1=0;i1<w;i1++)
		for (int i2=0;i2<h;i2++)
			field[x+i1][y+i2].wall(x+i1,y+i2);
	for (int i1=1;i1<w-1;i1++)
		for (int i2=1;i2<h-1;i2++)
			field[x+i1][y+i2].floor(x+i1,y+i2);
	switch (d)
	{
		case 'h':field[x][y+(h/2)].floor(x,y+(h/2));break;
		case 'j':field[x+(w/2)][y+h-1].floor(x+(w/2),y+h-1);break;
		case 'k':field[x+(w/2)][y].floor(x+(w/2),y+(h/2));break;
		case 'l':field[x+w-1][y+(h/2)].floor(x+w-1,y+(h/2));break;
	}
}
