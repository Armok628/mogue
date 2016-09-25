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
	field[x][y].set('@');
	field[x][y].setc(_L_BLUE);
	field[x][y].m=false;
	px=x;py=y;
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
	return 0;
}
int Display::dungeon(int x,int y,int w,int h,char d)
{
	bool n=true;
	/*
	for (int i=y;i<y+h;i++)
		if (field[x][i].fg=='%')
			n=false;
	if (n)
		for (int i=x;i<x+w;i++)
			if (field[i][y+h].fg=='%')
				n=false;
	if (n)
		for (int i=x;i<x+w;i++)
			if (field[i][y].fg=='%')
				n=false;
	if (n)
		for (int i=y;i<y+h;i++)
			if (field[x+w][i].fg=='%')
				n=false;
	*/
	for (int ix=x-1;ix<x+w+1;ix++)
	{
		if (!n)
			break;
		else for (int iy=y-1;iy<y+h+1;iy++)
			if (field[ix][iy].fg=='%'||field[ix][iy].bg=='#')
			{
				n=false;
				break;
			}
	}
	if (n)
		return house(x,y,w,h,d);
	else return 1;
}
void Display::cullwalls()
{
	int r;
	do
	{
		r=0;
		for (int x=1;x<63;x++)
			for (int y=1;y<31;y++)
			{
				if (field[x][y].fg=='%'&&(
						(field[x+1][y].bg=='#'&&field[x-1][y].bg=='#')||
						(field[x][y+1].bg=='#'&&field[x][y-1].bg=='#')))
				{
					field[x][y].floor(x,y);
					r++;
				}
			}
		//cout<<r<<endl;
	} while (r>0);
}
