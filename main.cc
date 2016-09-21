#include <iostream>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <ncurses.h>
#include "main.h"
#include "tile.h"
using namespace std;
static char _RESET_COLOR[]="\033[0m";
static char _RED[]="\033[1;31m";
static char _WHITE[]="\033[1;39m";
static bool quit = false;
static char input[64];
static char ione;
void draw();
int main()
{
	initscr();
	srand(time(0));
	for (int y=0;y<32;y++)
		for (int x=0;x<64;x++)
			field[x][y].grass();
	/*house. to-do: make display class*/
	for (int y=15;y<20;y++)
		for (int x=15;x<25;x++)
			field[x][y].wall();
	for (int y=16;y<19;y++)
		for (int x=16;x<24;x++)
			field[x][y].floor();
	field[15][17].floor();
	/**/
	field[10][10].set('@');field[10][10].setc(_WHITE);
	field[10][10].m=false;
	field[32][16].animal();
	px=10;py=10;
	do
	{
		clear();refresh();
		draw();
		ione=getch();
		if (ione=='q')
			quit=true;
		else field[px][py].pmove(ione);
		for (int y=0;y<32;y++)
			for (int x=0;x<64;x++)
				field[x][y].rmove(x,y);
	}
	while (quit==false);
	endwin();
	return 0;
}
void draw()
{
	cout<<endl;
	for (int y=0;y<32;y++)
	{
		for (int x=0;x<64;x++)
			cout<<field[x][y].retc()<<field[x][y].ret();
		cout<<_RESET_COLOR<<endl<<'\r';
	}
}
