#include <iostream>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <ncurses.h>
#include "main.h"
#include "display.h"
#include "tile.h"
using namespace std;
static bool quit = false;
static char input[64];
static char ione;
static Display d;
int main()
{
	initscr();
	srand(time(0));
	for (int y=0;y<32;y++)
		for (int x=0;x<64;x++)
			field[x][y].grass();
	d.statichouse();
	d.placeplayer(10,10);
	field[32][16].animal();
	field[33][16].animal();
	px=10;py=10;
	do
	{
		clear();
		refresh();
		d.draw();
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
