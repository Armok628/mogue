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
	cout<<endl;// Start a new line.
	for (int y=0;y<32;y++)// For every row:
	{
		for (int x=0;x<64;x++)// For every item on the row,
			cout<<field[x][y].retc()<<field[x][y].ret();// ...print its color and character.
		cout<<_RESET_COLOR<<endl<<'\r';// At the end of every row, print a carriage return.
	}
	
}
void Display::placeplayer(int x,int y)
{
	field[x][y].set('@');// Set the target tile's foreground to the player character (pun unintended)
	field[x][y].setc(_L_BLUE);// Set the player's color to light blue.
	field[x][y].m=false;// Do not allow the player to move randomly.
	px=x;py=y;// Set the player coordinate index.
}
int Display::house(int x,int y,int w,int h,char d)
{
	for (int i1=0;i1<w;i1++)// For every space of intended width...
		for (int i2=0;i2<h;i2++)// ...and every space of intended (figurative) height:
			field[x+i1][y+i2].wall(x+i1,y+i2);// Set each tile to a wall.
	for (int i1=1;i1<w-1;i1++)// For every space of intended internal width...
		for (int i2=1;i2<h-1;i2++)// ...and every space of intended internal height:
			field[x+i1][y+i2].floor(x+i1,y+i2);// Set each tile to a floor.
	switch (d)// Depending on the direction parameter:
	{
		case 'h':field[x][y+(h/2)].floor(x,y+(h/2));break;// If West, place a door in the middle of the West wall.
		case 'j':field[x+(w/2)][y+h-1].floor(x+(w/2),y+h-1);break;// If South, the South wall.
		case 'k':field[x+(w/2)][y].floor(x+(w/2),y+(h/2));break;// If North, the North wall.
		case 'l':field[x+w-1][y+(h/2)].floor(x+w-1,y+(h/2));break;// If East, the East wall.
	}
	return 0;
}
int Display::dungeon(int x,int y,int w,int h,char d)// This function creates a house surrounded by a perimeter of grass.
{
	bool n=true;// Set a local variable to represent that no tiles are in the way.
	for (int ix=x-1;ix<x+w+1;ix++)// For every column of intended dungeon,
	{
		if (!n)// If there is a tile in the way:
			break;// Cancel the whole operation.
		else for (int iy=y-1;iy<y+h+1;iy++)// Otherwise, for every row:
			if (field[ix][iy].fg=='%'||field[ix][iy].bg=='#')// If the tile is a floor or wall:
			{
				n=false;// There is now a tile in the way.
				break;// Cancel the current operation.
			}
	}
	if (n)// If there were no tiles in the way:
	{
		house(x,y,w,h,d);// Create the house.
		return 0;// Report success.
	}
	else return 1;// Otherwise, report failure.
}
void Display::cullwalls()
{
	int r;// Initialize a local variable counting walls removed per loop iteration.
	do// At least once:
	{
		r=0;// Reset the variable of walls removed at the start of this iteration.
		for (int x=1;x<63;x++)// For every column:
			for (int y=1;y<31;y++)// For every row:
			{
				if (field[x][y].fg=='%'&&(// If the tile is a wall and...
						(field[x+1][y].bg=='#'&&field[x-1][y].bg=='#')||// ...the tiles above & below are floors...
						(field[x][y+1].bg=='#'&&field[x][y-1].bg=='#')))// ...or the tiles side to side are floors:
				{
					field[x][y].floor(x,y);// Set that wall to a floor.
					r++;// Increase the number of walls removed.
				}
			}
	} while (r>0);// Continue until no more walls have been removed in an iteration.
}
