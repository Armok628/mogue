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
void Display::makedoors()
{
	for (int x1=0;x1<64;x1++)// For every column:
		for (int y1=0;y1<32;y1++)// For every row:
			if (field[x1][y1].bg=='#'&&// If the tile is a floor and...
					((field[x1-1][y1].fg=='%'&&field[x1+1][y1].fg=='%')!=// is flanked horizontally by walls,
					(field[x1][y1-1].fg=='%'&&field[x1][y1+1].fg=='%')))// xor vertically:
			{
				int c=0;// Start a new counter.
				for (int x2=-1;x2<=1;x2++)// For columns from negative one to one:
					for (int y2=-1;y2<=1;y2++)// For rows from negative one to one:
						if (field[x1+x2][y1+y2].fg=='%')// Relative to the floor, if the tile is a wall:
							c++;// Increase the counter.
				if (c<5)// If there are four or fewer walls adjacent to the floor:
					field[x1][y1].door();// Place a door on the tile.
			}
}
int Display::offset(char a,char c)
{
	int offset[2];offset[0]=0;offset[1]=0;// Initialize the offset coordinates.
	switch (c)// Act based on parameter c.
		{
			case '.':break;// If the input character is a period, let there be no offset.
			case 'h':offset[0]=-1;break;// If West, set X offset to negative one.
	 		case 'j':offset[1]=1;break;// If South, set Y offset to one.
			case 'k':offset[1]=-1;break;// If North, set Y offset to negative one.
			case 'l':offset[0]=1;break;// If East, set X offset to one.
			case 'y':offset[0]=-1;offset[1]=-1;break;// If Northwest, set both X and Y offsets to negative one.
	 		case 'u':offset[0]=1;offset[1]=-1;break;// If Northeast, set X offset to one, Y offset to negative one.
			case 'b':offset[0]=-1;offset[1]=1;break;// If Southwest, set X offset to negative one, Y offset to one.
			case 'n':offset[0]=1;offset[1]=1;break;// If Southeast, set both X and Y offsets to one.
	 		default :return 0;// If no relevant key was pressed, return zero.
		}
	if (a=='x')// If the offset requested was x:
		return offset[0];// Return the x offset.
	else if (a=='y')// Otherwise, if the offset requested was y:
		return offset[1];// Return the y offset.
	else return 0;// Otherwise, return zero.
}
